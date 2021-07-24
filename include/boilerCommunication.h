#ifndef BOILER_COMMUNICATION
#define BOILER_COMMUNICATION

#include "udp.h"

char * chopN( char *s, size_t n );
float read_sensor(char *msg, int socket_local, struct sockaddr_in endereco_destino);
void send_message(char *sensor_id, float value, int socket_local, struct sockaddr_in endereco_destino);

#endif