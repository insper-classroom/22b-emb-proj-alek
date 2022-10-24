/*
 * sound_capture.h
 *
 * Created: 10/5/2022 10:50:09 PM
 *  Author: Ricardo Ribeiro Rodrigues
 */

#ifndef SOUND_CAPTURE_H_
#define SOUND_CAPTURE_H_

#include "configs_io.h"

extern xSemaphoreHandle xSemaphoreGate;

// Variaveis globais -> ver como se livrar delas.
extern volatile uint32_t sdram_count;
extern uint32_t *g_sdram;

void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void RTT_Handler(void);
// void static AFEC_pot_Callback(void);

#endif /* SOUND_CAPTURE_H_ */