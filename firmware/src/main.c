/************************************************************************
* 5 semestre - Eng. da Computao - Insper
*
* 2021 - Exemplo com HC05 com RTOS
*
*/

#include <asf.h>
#include "conf_board.h"
#include <string.h>
#include <configs_io.h>
#include <math.h>

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

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_BLUETOOTH_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY (tskIDLE_PRIORITY)

// Queue para inputs dos botoes
QueueHandle_t xQueueInput;

// Timer para terminar a captura de som
// TimerHandle_t xTimerSound;

// Semaforo para o fim da captura de som
SemaphoreHandle_t xSemaphoreGate;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
volatile uint16_t *g_sdram = (uint16_t *)BOARD_SDRAM_ADDR;
volatile uint16_t sdram_count = 0;
volatile char enviando = 0;
// volatile _Bool gravando = 0;

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
extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

// Esse callback deve ser chamado quando o GATE ficar em estado logico baixo por 500ms.
void vTimerCallbackSound(TimerHandle_t xTimer) {
    // Desabilita o RTT -> Para a captura de som.
    // rtt_disable_interrupt(RTT, RTT_MR_ALMIEN);
    // rtt_disable(RTT);

    // Desabilita o AFEC
    // afec_disable_interrupt(AFEC_POT, AFEC_POT_CHANNEL);

    // Libera o semaforo do gate.
    xSemaphoreGiveFromISR(xSemaphoreGate, 0);
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
		xSemaphoreGiveFromISR(xSemaphoreGate, 0);
	}
}

static void AFEC_pot_callback(void) {
	uint16_t value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	
	*(g_sdram + sdram_count++) = value;
	pio_toggle_pin_group(TESTE_PIO, TESTE_IDX_MASK);
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
	uint32_t i ;

	while(*(pstring + i))
	if(uart_is_tx_empty(USART_COM))
	usart_serial_putchar(USART_COM, *(pstring+i++));
}

void usart_put_string(Usart *usart, char str[]) {
	usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
	uint timecounter = timeout_ms;
	uint32_t rx;
	uint32_t counter = 0;

	while( (timecounter > 0) && (counter < bufferlen - 1)) {
		if(usart_read(usart, &rx) == 0) {
			buffer[counter++] = rx;
		}
		else{
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
	config.baudrate = 9600;
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
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEAlek", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN1298", 100);
}

/**
 * Configura RTT
 *
 * arg0 pllPreScale  : Frequ�ncia na qual o contador ir� incrementar
 * arg1 IrqNPulses   : Valor do alarme
 * arg2 rttIRQSource : Pode ser uma
 *     - 0:
 *     - RTT_MR_RTTINCIEN: Interrup��o por incremento (pllPreScale)
 *     - RTT_MR_ALMIEN : Interrup��o por alarme
 */
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

    uint16_t pllPreScale = (int)(((float)32768) / freqPrescale);

    rtt_sel_source(RTT, false);
    rtt_init(RTT, pllPreScale);

    if (rttIRQSource & RTT_MR_ALMIEN) {
        uint32_t ul_previous_time;
        ul_previous_time = rtt_read_timer_value(RTT);
        while (ul_previous_time == rtt_read_timer_value(RTT))
            ;
        rtt_write_alarm_time(RTT, IrqNPulses + ul_previous_time);
    }

    /* config NVIC */
    NVIC_DisableIRQ(RTT_IRQn);
    NVIC_ClearPendingIRQ(RTT_IRQn);
    NVIC_SetPriority(RTT_IRQn, 4);
    NVIC_EnableIRQ(RTT_IRQn);

    /* Enable RTT interrupt */
    if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
        rtt_enable_interrupt(RTT, rttIRQSource);
    else
        rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
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

void task_bluetooth(void) {
	printf("Task Bluetooth started \n");
	
	printf("Inicializando HC05 \n");
	config_usart0();
	hc05_init();

	// configura LEDs e Botões
	io_init();

	char button1 = '0';
	char eof = 'X';

    // Task não deve retornar.
    while (1) {
        // atualiza valor do botão e envia apenas quando o valor muda através da fila.
        if ((xQueueReceive(xQueueInput, &button, 0)) && (!enviando)) {
            printf("Botao 1: %c \n", button);

            // Envia que o comando é um botão
            while (!usart_is_tx_ready(USART_COM)) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            usart_write(USART_COM, 'b');
			
			// Envia qual botao esta mandando
			while (!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, button);

            // envia fim de pacote
            while (!usart_is_tx_ready(USART_COM)) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            usart_write(USART_COM, eof);

            // dorme por 500 ms
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
	
		
        // TODO: Implementar aqui o envio de audio via bluetooth.  
		 if (xSemaphoreTake(xSemaphoreGate, 0) == pdTRUE) {
			 enviando = 1;
			 
			 // Envia comando som = 'S'
			 while (!usart_is_tx_ready(USART_COM)) {
				 vTaskDelay(10 / portTICK_PERIOD_MS);
			 }
			 usart_write(USART_COM, 'S');
			 
			 // Calcula tamanho do audio vai de 1 até 2 ^ 255
			 char tamanho = log2(sdram_count + 1);
			 
			 while (!usart_is_tx_ready(USART_COM)) {
				 vTaskDelay(10 / portTICK_PERIOD_MS);
			 }
			 usart_write(USART_COM, tamanho);
			 printf("%d : %d \n", sdram_count, tamanho);
			 		
			 
			for (uint16_t i = 0; i < sdram_count; i++){
				while (!usart_is_tx_ready(USART_COM)) {
				}

				char valor = *(g_sdram + i) >> 4;
				printf("%i : %d \n", i, valor);

				usart_write(USART_COM, valor);
			}
			
			// envia fim de pacote
			while (!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, eof);
			
			enviando = 0;
			
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
	
    xSemaphoreGate = xSemaphoreCreateBinary();
	if (xSemaphoreGate == NULL)
		printf("Erro ao criar o semaforo do gate\n");


	/* Create task to make led blink */
	xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL,	TASK_BLUETOOTH_STACK_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
