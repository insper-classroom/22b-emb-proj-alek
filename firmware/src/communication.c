/*
 * communication.c
 *
 * Created: 10/5/2022 10:44:49 PM
 *  Author: Ricardo Ribeiro Rodrigues
 */
#include "communication.h"


void send_pack(package msg, Usart *USART_COM) {
	// Envia o commando
	while (!usart_is_tx_ready(USART_COM)) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	usart_write(USART_COM, msg.command);
	
	// Envia o(s) dados do pacote
	
	while (!usart_is_tx_ready(USART_COM)) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	usart_write(USART_COM, msg.command);

	// Envia o EOF.
	while (!usart_is_tx_ready(USART_COM)) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	usart_write(USART_COM, EOF);
}


