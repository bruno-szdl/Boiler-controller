#ifndef MONITOR_VARIABLES
#define MONITOR_VARIABLES

#include <stdio.h>
#include <pthread.h>
#include "variables.h"


pthread_mutex_t variables_mutex;

void updateSensorsGlobalVar(struct sensors_struct *sensors, struct sensors_struct *sensors_data);
void updateActuatorsTemperatureGlobalVar(struct actuators_struct *actuators, struct actuators_struct *actuators_data);
void updateActuatorsHeightGlobalVar(struct actuators_struct *actuators, struct actuators_struct *actuators_data);
void updateReferenceGlobalVar(struct reference_struct *reference, struct reference_struct *reference_data);
void copyReference(struct reference_struct *reference, struct reference_struct *reference_data);
void copyAllData(struct sensors_struct *sensors,      struct actuators_struct *actuators,      struct reference_struct *reference,
				 struct sensors_struct *sensors_data, struct actuators_struct *actuators_data, struct reference_struct *reference_data);

#endif