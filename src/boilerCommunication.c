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


float read_sensor(char *msg, int local_socket, struct sockaddr_in dest_address){

	char sensor_value[1000];
	int nrec;

	send_msg(local_socket, dest_address, msg);

    nrec = get_msg(local_socket, sensor_value, 1000);

	chopN(sensor_value, 3);
    sensor_value[nrec] = '\0';

	char *ptr;
	double sensor_value_float = strtod(sensor_value, &ptr);

	return sensor_value_float;
}

void write_actuator(char *sensor_id, float value, int local_socket, struct sockaddr_in dest_address){

	char actuator_value[1000];
	int nrec;

	char char_value[10];
	sprintf (char_value, "%.1f", value);

	char msg[100] = "";
	strcat(msg, sensor_id);
	strcat(msg, char_value);

	send_msg(local_socket, dest_address, msg);

    nrec = get_msg(local_socket, actuator_value, 1000);

	chopN(actuator_value, 3);
    actuator_value[nrec] = '\0';

	char *ptr;
	double actuator_value_float = strtod(actuator_value, &ptr);
}