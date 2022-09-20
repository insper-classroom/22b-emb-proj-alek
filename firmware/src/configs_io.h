/*
 * configs_io.h
 *
 * Created: 20/09/2022 14:04:46
 *  Author: ricardorr7
 */ 


#ifndef CONFIGS_IO_H_
#define CONFIGS_IO_H_

#include <asf.h>

extern volatile char but_pause_flag;
extern volatile char but_retro_flag;
extern volatile char but_prox_flag;


// LEDs
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Botão verde - PROX musica
#define BUT_PROX_PIO      PIOB
#define BUT_PROX_PIO_ID   ID_PIOB
#define BUT_PROX_IDX      3
#define BUT_PROX_IDX_MASK (1 << BUT_PROX_IDX)

// Botão vermelho - PAUSE/PLAY
#define BUT_PAUSE_PIO      PIOA
#define BUT_PAUSE_PIO_ID   ID_PIOA
#define BUT_PAUSE_IDX      0
#define BUT_PAUSE_IDX_MASK (1 << BUT_PAUSE_IDX)

// Botão amarelo -> Retroceder musica
#define BUT_RETRO_PIO      PIOC
#define BUT_RETRO_PIO_ID   ID_PIOC
#define BUT_RETRO_IDX      31
#define BUT_RETRO_IDX_MASK (1 << BUT_RETRO_IDX)

void io_init(void);
void but_retro_callback(void);
void but_prox_callback(void);
void but_pause_callback(void);

#endif /* CONFIGS_IO_H_ */