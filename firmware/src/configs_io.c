/*
 * configs_io.c
 *
 * Created: 20/09/2022 14:06:03
 *  Author: ricardorr7
 */
#include "configs_io.h"

void but_prox_callback(void) {
    char but = 'R';
    xQueueSendFromISR(xQueueInput, &but, 0);
}

void but_pause_callback(void) {
    char but = 'P';
    xQueueSendFromISR(xQueueInput, &but, 0);
}

void but_retro_callback(void) {
    char but = 'T';
    xQueueSendFromISR(xQueueInput, &but, 0);
}

void power_callback(void) {
	printf("Chega aqui!");
}


void gate_callback(void) {
	/*
    if (pio_get(GATE_PIO, PIO_INPUT, GATE_IDX_MASK)) {
        // Aqui cabe também uma lógica para inicializar o RTT e o AFEC
        // se não estiverem inicializados.
		if (!gravando) {
			printf("Inicializa o negocio!\n");
			RTT_init(FREQ, 0, 0);
			config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
			gravando = 1;
		}
        // Borda de subida -> Para o timer
        xTimerStopFromISR(xTimerSound, 0);
    } else {
        // Borda de descida -> Reseta o timer para fim do som
        // Se ficar > 500 ms sem som, para a captura e envia por bluetooth.
        xTimerResetFromISR(xTimerSound, 0);
    }
	*/
}

void io_init(void) {

    // Ativa PIOs
    pmc_enable_periph_clk(LED_PIO_ID);
    pmc_enable_periph_clk(BUT_PIO_ID);
    pmc_enable_periph_clk(BUT_PROX_PIO_ID);
    pmc_enable_periph_clk(BUT_PAUSE_PIO_ID);
    pmc_enable_periph_clk(BUT_RETRO_PIO_ID);
    pmc_enable_periph_clk(GATE_PIO_ID);
    pmc_enable_periph_clk(TESTE_PIO_ID);
	pmc_enable_periph_clk(POWER_BUT_PIO);
	pmc_enable_periph_clk(POWER_LED_ID);
    
	

    // Configura Pinos
    pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
    pio_configure(TESTE_PIO, PIO_OUTPUT_0, TESTE_IDX_MASK, PIO_DEFAULT);
	
	// Configura saida do led do botao power
	pio_configure(POWER_LED_PIO, PIO_OUTPUT_0, POWER_LED_IDX_MASK, PIO_DEFAULT);
	
	// Saida do led e rele
	pio_configure(LED_OUT_PIO, PIO_OUTPUT_0, LED_OUT_IDX_MASK, PIO_DEFAULT);
    
	// Configura entradas
    pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 60);
	
    pio_configure(BUT_PROX_PIO, PIO_INPUT, BUT_PROX_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PROX_PIO, BUT_PROX_IDX_MASK, 60);
	
    pio_configure(BUT_PAUSE_PIO, PIO_INPUT, BUT_PAUSE_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PAUSE_PIO, BUT_PAUSE_IDX_MASK, 60);
	
    pio_configure(BUT_RETRO_PIO, PIO_INPUT, BUT_RETRO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_RETRO_PIO, BUT_RETRO_IDX_MASK, 60);
	
	pio_configure(POWER_BUT_PIO, PIO_INPUT, POWER_BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(POWER_BUT_PIO, POWER_BUT_IDX_MASK, 60);

    // Configurando o pino para ler o GATE do sound detector
    pio_configure(GATE_PIO, PIO_INPUT, GATE_IDX_MASK, PIO_DEFAULT);
    pio_set_debounce_filter(GATE_PIO, GATE_IDX_MASK, 60);

    // Interruptions handlers
    pio_handler_set(BUT_PROX_PIO,
                    BUT_PROX_PIO_ID,
                    BUT_PROX_IDX_MASK,
                    PIO_IT_RISE_EDGE,
                    but_prox_callback);

    pio_handler_set(BUT_PAUSE_PIO,
                    BUT_PAUSE_PIO_ID,
                    BUT_PAUSE_IDX_MASK,
                    PIO_IT_RISE_EDGE,
                    but_pause_callback);

    pio_handler_set(BUT_RETRO_PIO,
                    BUT_RETRO_PIO_ID,
                    BUT_RETRO_IDX_MASK,
                    PIO_IT_RISE_EDGE,
                    but_retro_callback);

    // Para o GATE
    pio_handler_set(GATE_PIO,
                    GATE_PIO_ID,
                    GATE_IDX_MASK,
                    PIO_IT_EDGE,
                    gate_callback);
					
	pio_handler_set(BUT_PIO,
					BUT_PIO_ID,
					BUT_IDX_MASK,
					PIO_IT_EDGE,
					but_callback);
					
	pio_handler_set(POWER_BUT_PIO,
					POWER_BUT_ID,
					POWER_BUT_IDX_MASK,
					PIO_IT_RISE_EDGE,
					power_callback);

    // Ativa interrupção e limpa primeira IRQ gerada na ativacao
    pio_enable_interrupt(BUT_PROX_PIO, BUT_PROX_IDX_MASK);
    pio_get_interrupt_status(BUT_PROX_PIO);

    pio_enable_interrupt(BUT_PAUSE_PIO, BUT_PAUSE_IDX_MASK);
    pio_get_interrupt_status(BUT_PAUSE_PIO);

    pio_enable_interrupt(BUT_RETRO_PIO, BUT_RETRO_IDX_MASK);
    pio_get_interrupt_status(BUT_RETRO_PIO);
	
	// PIO botao da placa
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);

    // Para o GATE
    pio_enable_interrupt(GATE_PIO, GATE_IDX_MASK);
    pio_get_interrupt_status(GATE_PIO);
	
	// Para o botao power
	pio_enable_interrupt(POWER_BUT_PIO, POWER_BUT_IDX_MASK);
	pio_get_interrupt_status(POWER_BUT_PIO);

    // Configura NVIC para receber interrupcoes do PIO do botao start/pause
    // com prioridade 4 (quanto mais proximo de 0 maior)
    NVIC_EnableIRQ(BUT_PROX_PIO_ID);
    NVIC_SetPriority(BUT_PROX_PIO_ID, 4);

    NVIC_EnableIRQ(BUT_PAUSE_PIO_ID);
    NVIC_SetPriority(BUT_PAUSE_PIO_ID, 4);

    NVIC_EnableIRQ(BUT_RETRO_PIO_ID);
    NVIC_SetPriority(BUT_RETRO_PIO_ID, 4);

    // Para o GATE
    NVIC_EnableIRQ(GATE_PIO_ID);
    NVIC_SetPriority(GATE_PIO_ID, 4);
	
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4);
	
	NVIC_EnableIRQ(POWER_BUT_ID);
	NVIC_SetPriority(POWER_BUT_ID, 4);
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