/*
 * sound_capture.c
 *
 * Created: 10/5/2022 10:50:20 PM
 *  Author: Ricardo Ribeiro Rodrigues
 */

#include "sound_capture.h"


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



/*
static void AFEC_pot_Callback(void) {
	uint32_t valor =  afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	printf("Valor = %u\n", valor);
    // Semaforo que indica o fim da captura.
    if (xSemaphoreTake(xSemaphoreGate, 0) == pdFALSE) {
        // Enquanto o gate nao ficar em nivel logico 0, continua a leitura do audio
		uint32_t valor =  afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
		printf("Valor = %u\n", valor);
		
        // *(g_sdram + sdram_count) = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
        // sdram_count++;
    } else {
		printf("AHHHHHHHHHHHHHHHHHHHHHHHHH\n");
        // Disabilita afec e zera a contagem do sdram
        afec_disable_interrupt(AFEC_POT, AFEC_POT_CHANNEL);
        // TODO: Idealizar como fazer um envio simultaneo, sem que tenha que parar o envio de audio
        // Idea: Fazer um semaforo aqui para mandar o ultimo endereço de memoria que
        // foi gravado esse bloco de audio
        // E continuar a gravar no proximo bloco de memoria nesse endeco + 1.
        // Quando estourar o tamanho do sdram, volta para o inicio.
        // sdram_count = 0;
    }
}
*/