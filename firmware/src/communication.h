/*
 * communication.h
 *
 * Created: 10/5/2022 10:44:37 PM
 *  Author: riywa
 */ 


#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

typedef struct{
	char head[4];
	char payload[255];
	char eop[2];
} package;

package pack(char *values_array, int n);

#endif /* COMMUNICATION_H_ */