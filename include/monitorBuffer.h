#ifndef MONITOR_BUFFER
#define MONITOR_BUFFER

#include <pthread.h>
#include <stdio.h>
#include "variables.h"

#define BUFFER_SIZE 360
#define N_VARIABLES 12

void insertInBuffer(double seconds, struct sensors_struct *sensors, struct actuators_struct *actuators, struct reference_struct *reference);
double *waitFullBuffer();

#endif