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
// extern TimerHandle_t xTimerSound;
// extern volatile _Bool gravando;
// extern SemaphoreHandle_t xSemaphoreGate;
// extern void but_callback(void);
// extern void AFEC_pot_Callback(void);
// extern void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);

// Teste afec
// static void AFEC_pot_Callback(void);


// LEDs
#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_IDX 8
#define LED_IDX_MASK (1 << LED_IDX)

// Botao da placa
#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_IDX 11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Botao verde - PROX musica
#define BUT_PROX_PIO PIOB
#define BUT_PROX_PIO_ID ID_PIOB
#define BUT_PROX_IDX 3
#define BUT_PROX_IDX_MASK (1 << BUT_PROX_IDX)

// Bot�o vermelho - PAUSE/PLAY
#define BUT_PAUSE_PIO PIOA
#define BUT_PAUSE_PIO_ID ID_PIOA
#define BUT_PAUSE_IDX 0
#define BUT_PAUSE_IDX_MASK (1 << BUT_PAUSE_IDX)

// Botao amarelo -> Retroceder musica
#define BUT_RETRO_PIO PIOC
#define BUT_RETRO_PIO_ID ID_PIOC
#define BUT_RETRO_IDX 31
#define BUT_RETRO_IDX_MASK (1 << BUT_RETRO_IDX)

// Sound detector
/* AFEC */
#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0 // Canal do pino PD30


// Gate do sound detector
#define GATE_PIO PIOA
#define GATE_PIO_ID ID_PIOA
#define GATE_IDX 6
#define GATE_IDX_MASK (1 << GATE_IDX)

// Para debug da onda som
#define TESTE_PIO PIOC
#define TESTE_PIO_ID ID_PIOC
#define TESTE_IDX 19
#define TESTE_IDX_MASK (1 << TESTE_IDX)

// Para leitura de botao power
#define POWER_BUT_PIO PIOD
#define POWER_BUT_ID ID_PIOD
#define POWER_BUT_IDX 28
#define POWER_BUT_IDX_MASK (1 << POWER_BUT_IDX)

// Para o led do botao power
#define POWER_LED_PIO PIOC
#define POWER_LED_ID ID_PIOC
#define POWER_LED_IDX 17
#define POWER_LED_IDX_MASK (1 << POWER_LED_IDX)

// Para controlar o led e rele
#define LED_OUT_PIO PIOA
#define LED_OUT_ID ID_PIOA
#define LED_OUT_IDX 19
#define LED_OUT_IDX_MASK (1 << LED_OUT_IDX)

// Freq de amostragem
#define FREQ 8000

void io_init(void);
void but_callback(void);
void but_retro_callback(void);
void but_prox_callback(void);
void but_pause_callback(void);
void gate_callback(void);
void power_callback(void);
// void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);

#endif /* CONFIGS_IO_H_ */