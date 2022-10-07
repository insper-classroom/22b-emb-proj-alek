/*
 * communication.c
 *
 * Created: 10/5/2022 10:44:49 PM
 *  Author: riywa
 */ 
#include "communication.h"

package pack(char *values_array, int start_value, int payload_size, int n_payloads) {
	char n_p1 = (char) n_payloads;
	n_payloads = n_payloads >> 8;
	char n_p2 = (char) n_payloads; 
	char head[] = {'s', (char) payload_size, n_p1, n_p2};
	char payload[payload_size];
	for (int i = 0; i < payload_size; i++) {
		payload_size[i] = values_array[i + start_value];
	}
	char eop[] = {'A', 'Z'};
	package ret_val = {head, payload, eop};
	return ret_val;
}

void 