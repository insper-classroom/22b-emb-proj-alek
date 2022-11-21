/*
 * send_bt.h
 *
 * Created: 11/20/2022 10:35:29 PM
 *  Author: riywa
 */ 


#ifndef SEND_BT_H_
#define SEND_BT_H_

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



#endif /* SEND_BT_H_ */