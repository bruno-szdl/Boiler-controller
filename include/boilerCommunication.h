#ifndef BOILER_COMMUNICATION
#define BOILER_COMMUNICATION

#include "udp.h"

char * chopN( char *s, size_t n );
float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address);
void write_actuator(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address);

#endif