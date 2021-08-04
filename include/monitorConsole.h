#ifndef MONITOR_CONSOLE
#define MONITOR_CONSOLE

#include <pthread.h>
#include <stdio.h>

#include "variables.h"
#include "monitorVariables.h"

pthread_mutex_t console_mutex;

void consoleData(struct sensors_struct *sensors_data, struct actuators_struct *actuators_data, struct reference_struct *reference_data);
void consoleGetReferenceValues(struct reference_struct *reference_data);
void consoleTemperatureAlarm(double T);
void consoleHeightAlarm(double H);

#endif