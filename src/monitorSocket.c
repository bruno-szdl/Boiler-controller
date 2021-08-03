#include "../include/monitorSocket.h"

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

void getSensorData(struct sensors_struct *sensors_data)
{
	pthread_mutex_lock(&socket_mutex);
	sensors_data->Ta = read_sensor("sta0", local_socket, dest_address);
	sensors_data->T = read_sensor("st-0", local_socket, dest_address);
	sensors_data->Ti = read_sensor("sti0", local_socket, dest_address);
	sensors_data->No = read_sensor("sno0", local_socket, dest_address);
	sensors_data->H = read_sensor("sh-0", local_socket, dest_address);
	pthread_mutex_unlock(&socket_mutex);
}

void setTemperatureActuators(struct actuators_struct *actuators_data)
{	
	pthread_mutex_lock(&socket_mutex);
	actuators_data->Q = actuate("aq-\0", actuators_data->Q, local_socket, dest_address);
	actuators_data->Na = actuate("ana\0", actuators_data->Na, local_socket, dest_address);
	actuators_data->Ni = actuate("ani\0", actuators_data->Ni, local_socket, dest_address);
	pthread_mutex_unlock(&socket_mutex);
}

void setHeightActuators(struct actuators_struct *actuators_data)
{
	pthread_mutex_lock(&socket_mutex);
	actuators_data->Na = actuate("ana\0", actuators_data->Na, local_socket, dest_address);
	actuators_data->Ni = actuate("ani\0", actuators_data->Ni, local_socket, dest_address);
	actuators_data->Nf = actuate("anf\0", actuators_data->Nf, local_socket, dest_address);
	pthread_mutex_unlock(&socket_mutex);
}


