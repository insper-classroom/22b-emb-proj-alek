/************************************************************************
 * 5 semestre - Eng. da Computao - Insper
 *
 * 2021 - Exemplo com HC05 com RTOS
 *
 */

#include <is42s16100e.h>
#include <configs_io.h>
#include <string.h>
#include <math.h>
#include "send_bt.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// usart (bluetooth ou serial)
// Descomente para enviar dados
// pela serial debug

//#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define USART_COM USART1
#define USART_COM_ID ID_USART1
#else
#define USART_COM USART0
#define USART_COM_ID ID_USART0
#endif

#define DEBUG 1
#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_BLUETOOTH_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY (tskIDLE_PRIORITY)

// Task para receber informacao do computador
#define TASK_RECEIVE_BT_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVE_BT_STACK_PRIORITY (tskIDLE_PRIORITY)

// Queue para inputs dos botoes
QueueHandle_t xQueueInput;

// Queue para conectar var global com sistema operacional
QueueHandle_t xQueueCount;

// Timer para piscar o botao de power !
TimerHandle_t xTimerBotao;

// Semaforo para o fim da captura de som
SemaphoreHandle_t xSemaphoreGate;

// Semaforo para sinalizar que esta conectado
SemaphoreHandle_t xSemaphoreConect;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
volatile uint16_t *g_sdram = (uint16_t *)BOARD_SDRAM_ADDR;
volatile uint16_t sdram_count = 0;
volatile char enviando = 0;

/************************************************************************/
/* RTOS application HOOK                                                */
/************************************************************************/

/* Called if stack overflow during execution */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName) {
    printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
    /* If the parameters have been corrupted then inspect pxCurrentTCB to
     * identify which task has overflowed its stack.
     */
    for (;;) {
    }
}

/* This function is called by FreeRTOS idle task */
extern void vApplicationIdleHook(void) {
    pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/* This function is called by FreeRTOS each tick */
extern void vApplicationTickHook(void) {}

extern void vApplicationMallocFailedHook(void) {
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT((volatile void *)NULL);
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void vTimerCallbackBotao(TimerHandle_t xTimer) {
	pio_toggle_pin_group(POWER_LED_PIO, POWER_LED_IDX_MASK);
}

void but_callback(void) {
	if ((!pio_get(BUT_PIO, PIO_INPUT, BUT_IDX_MASK)) && (!enviando)) {
		RTT_init(FREQ, 0, RTT_MR_RTTINCIEN);
		afec_enable_interrupt(AFEC_POT, AFEC_POT_CHANNEL);
		sdram_count = 0;
	} else {
		// Para de coletar o audio
		rtt_disable_interrupt(RTT, RTT_MR_ALMIEN | RTT_MR_RTTINCIEN);
		afec_disable_interrupt(AFEC_POT, AFEC_POT_CHANNEL);
		xQueueSendFromISR(xQueueCount, &sdram_count, 0);
	}
}

static void AFEC_pot_callback(void) {
	uint16_t value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	*(g_sdram + sdram_count++) = value;
	// Usado para debugar a freq de amostragem
	#ifdef DEBUG
	pio_toggle_pin_group(TESTE_PIO, TESTE_IDX_MASK);
	#endif // DEBUG

}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

static void configure_console(void) {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
#if (defined CONF_UART_CHAR_LENGTH)
        .charlength = CONF_UART_CHAR_LENGTH,
#endif
        .paritytype = CONF_UART_PARITY,
#if (defined CONF_UART_STOP_BITS)
        .stopbits = CONF_UART_STOP_BITS,
#endif
    };

    /* Configure console UART. */
    stdio_serial_init(CONF_UART, &uart_serial_options);

/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
    setbuf(stdout, NULL);
#else
/* Already the case in IAR's Normal DLIB default configuration: printf()
 * emits one character at a time.
 */
#endif
}

uint32_t usart_puts(uint8_t *pstring) {
    uint32_t i;

    while (*(pstring + i))
        if (uart_is_tx_empty(USART_COM))
            usart_serial_putchar(USART_COM, *(pstring + i++));
}

void usart_put_string(Usart *usart, char str[]) {
    usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
    uint timecounter = timeout_ms;
    uint32_t rx;
    uint32_t counter = 0;

    while ((timecounter > 0) && (counter < bufferlen - 1)) {
        if (usart_read(usart, &rx) == 0) {
            buffer[counter++] = rx;
        } else {
            timecounter--;
            vTaskDelay(1);
        }
    }
    buffer[counter] = 0x00;
    return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen,
                        char buffer_tx[], int timeout) {
    usart_put_string(usart, buffer_tx);
    usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void config_usart0(void) {
    sysclk_enable_peripheral_clock(ID_USART0);
    usart_serial_options_t config;
	// Necessita de configurar o modulo para 115200 de baudrate antes!
    config.baudrate = 115200;
    config.charlength = US_MR_CHRL_8_BIT;
    config.paritytype = US_MR_PAR_NO;
    config.stopbits = false;
    usart_serial_init(USART0, &config);
    usart_enable_tx(USART0);
    usart_enable_rx(USART0);

    // RX - PB0  TX - PB1
    pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
    pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_init(void) {
    char buffer_rx[128];
    usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEAlek", 100);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN1298", 100);
}

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
                            afec_callback_t callback) {
  /*************************************
   * Ativa e configura AFEC
   *************************************/
  /* Ativa AFEC - 0 */
  afec_enable(afec);

  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /*
  * Calibracao:
  * Because the internal ADC offset is 0x200, it should cancel it and shift
  down to 0.
  */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);

  /***  Configura sensor de temperatura ***/
  struct afec_temp_sensor_config afec_temp_sensor_cfg;

  afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
  afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);

  /* configura IRQ */
  afec_set_callback(afec, afec_channel, callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

void RTT_Handler(void) {
    uint32_t ul_status;
    ul_status = rtt_get_status(RTT);
   
	 /* IRQ due to Inc */
	 if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		 // Coleta do audio do microfone usando o AFEC1
		 afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
		 afec_start_software_conversion(AFEC_POT);
	 }
	 
    /* IRQ due to Alarm */
    if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		
    }
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

// Task para envio de informações para o computador
void task_bluetooth(void) {
    printf("Task Bluetooth started \n");

    printf("Inicializando HC05 \n");
    config_usart0();
    hc05_init();

    // configura LEDs e Botões
    io_init();


    char button;
    char eof = 'X';
	uint16_t count;
	char sleeping = 0;
	
	// Enquanto nao esta conectado, espera o semaforo da task_bt
	while (1) {
		if (xSemaphoreTake(xSemaphoreConect, 0) == pdTRUE) {
			break;
			debug_print("%s\n", "Inicia a task de envio");
		}
		vTaskDelay(100);
	}

    // Task não deve retornar.
    while (1) {
        // atualiza valor do botão e envia apenas quando o valor muda atravez da fila.
        if (xQueueReceive(xQueueInput, &button, 0)) {
			debug_print("Botao 1: %c\n", button);

			package pack;
			pack.comm = 'b';
			pack.button = button;
			send_data(pack, USART_COM);
			
			if (button == 'L') {
				if (!sleeping) {
					sleeping = 1;
					// Entra em sleep mode
					pio_clear(POWER_LED_PIO, POWER_LED_IDX_MASK);
					pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
				} else {
					pio_set(POWER_LED_PIO, POWER_LED_IDX_MASK);
					sleeping = 0;
				}
			}
        }
	
		
		if (xQueueReceive(xQueueCount, &count, 0)) {
			debug_print("%s %d\n", "Tamanho: ", count);
					
			enviando = 1;
			package pack;
			pack.comm = 'S';
			pack.count = count;
			send_data(pack, USART_COM);
			enviando = 0;
        }
    }
}

// Task dedicada a receber informações do computador.
void task_receive_bt(){
	char conectado = 0;
	xTimerBotao = xTimerCreate("TimerBotao",
								500 / portTICK_PERIOD_MS,
								pdTRUE,
								(void *)1,
								vTimerCallbackBotao);
								
	xTimerStart(xTimerBotao, 1);
								
	while(1) {
		// Enquanto nao termina o handshake, pisca o led
		if (!conectado) {
			if (usart_is_rx_ready(USART_COM)) {
				uint32_t dado;
				usart_read(USART_COM, &dado);
				if ((char) dado == 'H') {
					while (!usart_is_tx_ready(USART_COM)) {
						vTaskDelay(2 / portTICK_PERIOD_MS);
					}
					usart_write(USART_COM, 'O');
					debug_print("%s\n", "Terminou o handshake!");
					xSemaphoreGive(xSemaphoreConect);
					conectado = 1;
					xTimerStop(xTimerBotao, 1);
					pio_set(POWER_LED_PIO, POWER_LED_IDX_MASK);
				}
			}
		} else {
			if (usart_is_rx_ready(USART_COM)) {
				uint32_t dado;
				usart_read(USART_COM, &dado);
				// Computador iniciou modo fim de semana
				if ((char) dado == 'F') {
					pio_set(LED_OUT_PIO, LED_OUT_IDX_MASK);
				}
				// Computador encerrou o modo fim de semana
				else if ((char) dado == 'f') {
					pio_clear(LED_OUT_PIO, LED_OUT_IDX_MASK);
				}
			}
        }
	}
	
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
    /* Initialize the SAM system */
    sysclk_init();
    board_init();

    configure_console();
	
	config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_callback);
	
	/* Complete SDRAM configuration */
	pmc_enable_periph_clk(ID_SDRAMC);
	sdramc_init((sdramc_memory_dev_t *)&SDRAM_ISSI_IS42S16100E,	sysclk_get_cpu_hz());
	sdram_enable_unaligned_support();
	SCB_CleanInvalidateDCache();

    // Cria a queue de input
    xQueueInput = xQueueCreate(10, sizeof(char));
    if (xQueueInput == NULL) {
        printf("Erro ao criar a queue de input \n");
    }
	
	xQueueCount = xQueueCreate(2, sizeof(uint16_t));
	if (xQueueCount == NULL) {
		printf("Erro ao criar a queue de contagem\n");
	}
	
    xSemaphoreGate = xSemaphoreCreateBinary();
	if (xSemaphoreGate == NULL)
		printf("Erro ao criar o semaforo do gate\n");
	
	xSemaphoreConect = xSemaphoreCreateBinary();
	if (xSemaphoreConect == NULL)
		printf("Erro ao criar o semaforo do conect\n");

    /* Create task to make led blink */
	if (xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL, TASK_BLUETOOTH_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create task BLT\r\n");
	}
	
	if (xTaskCreate(task_receive_bt, "RBLT", TASK_RECEIVE_BT_STACK_SIZE, NULL, TASK_RECEIVE_BT_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create task RBLT\r\n");
	}

    /* Start the scheduler. */
    vTaskStartScheduler();

    while (1) {
    }

    /* Will only get here if there was insufficient memory to create the idle task. */
    return 0;
}
