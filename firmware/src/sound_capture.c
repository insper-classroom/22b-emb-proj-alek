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
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

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

void RTT_Handler(void) {
    uint32_t ul_status;
    ul_status = rtt_get_status(RTT);

    // Captura o audio
    afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
    afec_start_software_conversion(AFEC_POT);

    /* IRQ due to Alarm */
    if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
    }
}

static void AFEC_pot_Callback(void) {
    // Semaforo que indica o fim da captura.
    if (xSemaphoreTake(xSemaphoreGate, 0) == pdFALSE) {
        // Enquanto o gate nao ficar em nivel logico 0, continua a leitura do audio
        *(g_sdram + sdram_count) = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
        sdram_count++;
    } else {
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