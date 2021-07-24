#include "../include/boilerCommunication.h"

char * chopN(char *s, size_t n)
{
    char *src = s;

    while (*src && n) --n, ++src;

    if (n == 0 && src != s)
    {
        for (char *dst = s; (*dst++ = *src++); );
    }
    return s;
} 


float read_sensor(char *msg, int socket_local, struct sockaddr_in endereco_destino){

	char msg_recebida[1000];
	int nrec;

	envia_mensagem(socket_local, endereco_destino, msg);

    nrec = recebe_mensagem(socket_local, msg_recebida, 1000);

	chopN(msg_recebida, 3);
    msg_recebida[nrec] = '\0';

	char *ptr;
	double msg_recebida_float = strtod(msg_recebida, &ptr);

	return msg_recebida_float;
}

void send_message(char *sensor_id, float value, int socket_local, struct sockaddr_in endereco_destino){

	char msg_recebida[1000];
	int nrec;

	char char_value[10];
	sprintf (char_value, "%.1f", value);

	char msg[100] = "";
	strcat(msg, sensor_id);
	strcat(msg, char_value);

	envia_mensagem(socket_local, endereco_destino, msg);

    nrec = recebe_mensagem(socket_local, msg_recebida, 1000);

	chopN(msg_recebida, 3);
    msg_recebida[nrec] = '\0';

	char *ptr;
	double msg_recebida_float = strtod(msg_recebida, &ptr);
}