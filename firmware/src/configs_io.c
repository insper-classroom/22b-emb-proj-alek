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

    // Configura Pinos
    pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
    pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 60);
    pio_configure(BUT_PROX_PIO, PIO_INPUT, BUT_PROX_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PROX_PIO, BUT_PROX_IDX_MASK, 60);
    pio_configure(BUT_PAUSE_PIO, PIO_INPUT, BUT_PAUSE_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PAUSE_PIO, BUT_PAUSE_IDX_MASK, 60);
    pio_configure(BUT_RETRO_PIO, PIO_INPUT, BUT_RETRO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_RETRO_PIO, BUT_RETRO_IDX_MASK, 60);

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

    // Configura NVIC para receber interrupcoes do PIO do botao start/pause
    // com prioridade 4 (quanto mais pr�ximo de 0 maior)
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
}

