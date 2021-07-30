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
};

float communicate(char *msg, int local_socket, struct sockaddr_in dest_address){

	char value[1000];
	int nrec;

	send_msg(local_socket, dest_address, msg);

    nrec = get_msg(local_socket, value, 1000);

	chopN(value, 3);
    value[nrec] = '\0';

	char *ptr;
	double value_float = strtod(value, &ptr);

	return value_float;
};

float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address){

	return communicate(msg, local_socket, dest_address);
};

float actuate(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address){

	char char_value[10];
	sprintf(char_value, "%.1f", value);

	char msg[100] = "";
	strcat(msg, sensor_id);
	strcat(msg, char_value);

	return communicate(msg, local_socket, dest_address);
};