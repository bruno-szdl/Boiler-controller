#ifndef BOILER_COMMUNICATION
#define BOILER_COMMUNICATION

#include "udp.h"

char * chopN( char *s, size_t n );
float communicate(char *msg, int local_socket, struct sockaddr_in dest_address);
float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address);
float actuate(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address);

#endif