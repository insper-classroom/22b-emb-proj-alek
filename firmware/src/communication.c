/*
 * communication.c
 *
 * Created: 10/5/2022 10:44:49 PM
 *  Author: Ricardo Ribeiro Rodrigues
 */
#include "communication.h"

void send_data(package pack, Usart *p_usart) {
	// Envia commando
	while (!usart_is_tx_ready(p_usart)) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	usart_write(p_usart, pack.comm);
	
	// Dependendo do tipo de dado a enviar, usa função adequada
	if (pack.comm == 'S') {
		send_sound(pack.count, p_usart);
	} else if (pack.comm == 'b') {
		send_button(pack.button, p_usart);
	}

	
	// envia fim de pacote
	while (!usart_is_tx_ready(p_usart)) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	usart_write(p_usart, EOF);
	
}

void send_sound(uint16_t count, Usart *p_usart) {
	 // 5 bytes para o tamanho
	 char t[5];
	 t[0] = (char) count;
	 t[1] = (char) (count >> 8);
	 t[2] = (char) (count >> 16);
	 t[3] = (char) (count >> 24);
	 t[4] = 'T';
	 
	 for (int i = 0; i < 5; i++) {
		 while (!usart_is_tx_ready(p_usart)) {
		 }
		 usart_write(p_usart, t[i]);
		 #ifdef DEBUG
		 printf("%d : %d \n", count, t[i]);
		 #endif
	 }
	 
	 
	 for (uint16_t i = 0; i < count; i++){
		 while (!usart_is_tx_ready(p_usart)) {
		 }

		 char valor = *(g_sdram + i) >> 4;

		 usart_write(p_usart, valor);
	 }
}

void send_button(char button, Usart *p_usart) {
	// Envia qual botao esta mandando
	while (!usart_is_tx_ready(p_usart)) {
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
	usart_write(p_usart, button);
}


