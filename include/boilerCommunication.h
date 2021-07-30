#ifndef BOILER_COMMUNICATION
#define BOILER_COMMUNICATION

#include "udp.h"

char * chopN( char *s, size_t n );                                                              //Get only numbers from msg
float communicate(char *msg, int local_socket, struct sockaddr_in dest_address);                //communicate with the boiler using UDP functions
float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address);                //read sensor values from the boiler
float actuate(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address); //send actuator commands to the boiler

#endif