/*
 * communication.h
 *
 * Created: 10/5/2022 10:44:37 PM
 *  Author: Ricardo Ribeiro Rodigues
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
// Arquivo para definir o protocolo de comunicacao entre o microcontrolador e o computador

#include "configs_io.h"

extern volatile uint16_t *g_sdram;
extern volatile char enviando;

typedef struct {
    char comm;
	union {
		char button;
		uint16_t count;
	};
} package;

#define EOF 'X'

void send_data(package pack, Usart *p_usart);
void send_sound(uint16_t count, Usart *p_usart);
void send_button(char button, Usart *p_usart);


#endif /* COMMUNICATION_H_ */