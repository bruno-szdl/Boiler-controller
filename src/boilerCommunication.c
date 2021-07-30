#include "../include/boilerCommunication.h"


// Get only numbers from msg
char * getMsgNumbers(char *s){

	size_t n = 3;
    char *src = s;

    while (*src && n) --n, ++src;

    if (n == 0 && src != s){
        for (char *dst = s; (*dst++ = *src++); );
    }
    return s;
};


// Communicate with the boiler using UDP functions (send and receive msg)
float communicate(char *msg, int local_socket, struct sockaddr_in dest_address){

	char value[1000];
	int nrec;

	send_msg(local_socket, dest_address, msg);

    nrec = get_msg(local_socket, value, 1000);

	getMsgNumbers(value);
    value[nrec] = '\0';

	char *ptr;
	double value_float = strtod(value, &ptr);

	return value_float;
};


// Read sensor values from the boiler
float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address){
	return communicate(msg, local_socket, dest_address);
};


// Send actuator commands to the boiler
float actuate(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address){

	char char_value[10];
	sprintf(char_value, "%.1f", value);

	char msg[100] = "";
	strcat(msg, sensor_id);
	strcat(msg, char_value);

	return communicate(msg, local_socket, dest_address);
};