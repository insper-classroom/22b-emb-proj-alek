/*
 * communication.h
 *
 * Created: 10/5/2022 10:44:37 PM
 *  Author: Ricardo Ribeiro Rodigues
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
// Arquivo para definir o protocolo de comunicacao entre o microcontrolador e o computador

#include <configs_io.h>

#define EOF 'X'

typedef struct {
	char command;
	char *data;
	char eop;
} package;

void send_pack(package msg,  Usart *USART_COM);

#endif /* COMMUNICATION_H_ */