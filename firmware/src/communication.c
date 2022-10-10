/*
 * communication.c
 *
 * Created: 10/5/2022 10:44:49 PM
 *  Author: Ricardo Ribeiro Rodrigues
 */
#include "communication.h"

package pack(char *values_array, int start_value, int payload_size, int n_payload) {
    char head[4];
    // Pega os dois primeiros bytes do payload e coloca no head formato little endian.
    char n_p1 = (char)n_payloads;
    n_payloads = n_payloads >> 8;
    char n_p2 = (char)n_payloads;
    // Coloca o tipo do pacote no head.
    if (n_payload > 0) {
        head = {0x00, (char)payload_size, n_p1, n_p2};
    } else {
        head = {0x01, (char)payload_size, n_p1, n_p2};
    }
    // Monta o payload.
    char payload[payload_size];
    for (int i = 0; i < payload_size; i++) {
        payload_size[i] = values_array[i + start_value];
    }
    // Por fim o eop.
    char eop[] = {'A', 'Z'};
    package ret_val = {head, payload, eop};
    return ret_val;
}
