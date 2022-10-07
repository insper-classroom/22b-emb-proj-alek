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

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback){
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
	afec_set_trigger(AFEC0, AFEC_TRIG_TIO_CH_0);
	AFEC0->AFEC_MR |= 3;

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
	afec_set_callback(afec, afec_channel,	callback, 1);
	NVIC_SetPriority(afec_id, 4);
	NVIC_EnableIRQ(afec_id);
}