/*
 * communication.h
 *
 * Created: 10/5/2022 10:44:37 PM
 *  Author: Ricardo Ribeiro Rodigues
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
// Arquivo para definir o protocolo de comunicacao entre o microcontrolador e o computador

typedef struct {
    char head[4];
    char payload[255];
    char eop[2];
} package;

/*
    O head é formado por -> 1 byte de tipo + 1 byte de tamanho do payload + 2 bytes de numero do pacote.
    Esse byte de tipo pode ser:
        0x00 -> pacote de novo som.
        0x01 -> pacote de som incremental (continua a mandar um som que já estava enviando).
        0x02 -> pacote de botao (representando um dos botoes do projeto).
        0x03 -> pacote de erro
        0x04 -> pacote de fim de envio de som (No computador -> cria o arquivo de som e interpreta comandos).
        ... -> outros tipos de pacotes podem ser adicionados (Respostas do computador por ex).
        0xff -> Resposta de OK.
*/

package pack(char *values_array, int start_value, int payload_size, int n_payload);

#endif /* COMMUNICATION_H_ */