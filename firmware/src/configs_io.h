/*
 * configs_io.h
 *
 * Created: 20/09/2022 14:04:46
 *  Author: ricardorr7
 */

#ifndef CONFIGS_IO_H_
#define CONFIGS_IO_H_

#include <asf.h>

// Variaveis do RTOS externas
extern QueueHandle_t xQueueInput;
extern TimerHandle_t xTimerSound;
extern volatile _Bool gravando;
extern SemaphoreHandle_t xSemaphoreGate;
extern void but_callback(void);
// extern void AFEC_pot_Callback(void);
extern void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);

// Teste afec
// static void AFEC_pot_Callback(void);


// LEDs
#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_IDX 8
#define LED_IDX_MASK (1 << LED_IDX)

// Bot�o
#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_IDX 11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Bot�o verde - PROX musica
#define BUT_PROX_PIO PIOB
#define BUT_PROX_PIO_ID ID_PIOB
#define BUT_PROX_IDX 3
#define BUT_PROX_IDX_MASK (1 << BUT_PROX_IDX)

// Bot�o vermelho - PAUSE/PLAY
#define BUT_PAUSE_PIO PIOA
#define BUT_PAUSE_PIO_ID ID_PIOA
#define BUT_PAUSE_IDX 0
#define BUT_PAUSE_IDX_MASK (1 << BUT_PAUSE_IDX)

// Bot�o amarelo -> Retroceder musica
#define BUT_RETRO_PIO PIOC
#define BUT_RETRO_PIO_ID ID_PIOC
#define BUT_RETRO_IDX 31
#define BUT_RETRO_IDX_MASK (1 << BUT_RETRO_IDX)

// Sound detector
/* AFEC */


// Gate do sound detector
#define GATE_PIO PIOA
#define GATE_PIO_ID ID_PIOA
#define GATE_IDX 6
#define GATE_IDX_MASK (1 << GATE_IDX)

// Freq de amostragem
#define FREQ 6000

void io_init(void);
void but_callback(void);
void but_retro_callback(void);
void but_prox_callback(void);
void but_pause_callback(void);
void gate_callback(void);
// void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);

#endif /* CONFIGS_IO_H_ */