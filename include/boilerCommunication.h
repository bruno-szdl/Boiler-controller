#ifndef BOILER_COMMUNICATION
#define BOILER_COMMUNICATION

#include "udp.h"

char * getMsgNumbers( char *s);                                                                 // Get only the numbers from msg
float communicate(char *msg, int local_socket, struct sockaddr_in dest_address);                // Communicate with the boiler using UDP functions (send and receive msg)
float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address);                // Read sensor values from the boiler
float actuate(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address); // Send actuator commands to the boiler

#endif