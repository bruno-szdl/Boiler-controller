#ifndef MONITOR_SOCKET
#define MONITOR_SOCKET

#include <pthread.h>

#include "../include/boilerCommunication.h"
#include "variables.h"

pthread_mutex_t socket_mutex;

void getSensorData(struct sensors_struct *sensors_data);
void setTemperatureActuators(struct actuators_struct *actuators_data);
void setHeightActuators(struct actuators_struct *actuators_data);

#endif