/*
 * configs_io.c
 *
 * Created: 20/09/2022 14:06:03
 *  Author: ricardorr7
 */ 
#include "configs_io.h"

void but_prox_callback(void) {
	but_prox_flag = 1;
}

void but_pause_callback(void) {
	but_pause_flag = !but_pause_flag;
}

void but_retro_callback(void) {
	but_retro_flag = 1;
}



void io_init(void) {

	// Ativa PIOs
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT_PROX_PIO_ID);
	pmc_enable_periph_clk(BUT_PAUSE_PIO_ID);
	pmc_enable_periph_clk(BUT_RETRO_PIO_ID);

	// Configura Pinos
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT_PROX_PIO, PIO_INPUT, BUT_PROX_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT_PAUSE_PIO, PIO_INPUT, BUT_PAUSE_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT_RETRO_PIO, PIO_INPUT, BUT_RETRO_IDX_MASK, PIO_PULLUP);
	
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
	 
	  // Ativa interrupção e limpa primeira IRQ gerada na ativacao
	  pio_enable_interrupt(BUT_PROX_PIO, BUT_PROX_IDX_MASK);
	  pio_get_interrupt_status(BUT_PROX_PIO);
	  
	  pio_enable_interrupt(BUT_PAUSE_PIO, BUT_PAUSE_IDX_MASK);
	  pio_get_interrupt_status(BUT_PAUSE_PIO);
	  
	  pio_enable_interrupt(BUT_RETRO_PIO, BUT_RETRO_IDX_MASK);
	  pio_get_interrupt_status(BUT_RETRO_PIO);
	  
	  // Configura NVIC para receber interrupcoes do PIO do botao start/pause
	  // com prioridade 4 (quanto mais próximo de 0 maior)
	  NVIC_EnableIRQ(BUT_PROX_PIO_ID);
	  NVIC_SetPriority(BUT_PROX_PIO_ID, 4);
	  
	  NVIC_EnableIRQ(BUT_PAUSE_PIO_ID);
	  NVIC_SetPriority(BUT_PAUSE_PIO_ID, 4);
	  
	  NVIC_EnableIRQ(BUT_RETRO_PIO_ID);
	  NVIC_SetPriority(BUT_RETRO_PIO_ID, 4);	
}