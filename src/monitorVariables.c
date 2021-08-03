#include "../include/monitorVariables.h"

pthread_mutex_t variables_mutex = PTHREAD_MUTEX_INITIALIZER;

void updateSensorsGlobalVar(struct sensors_struct *sensors, struct sensors_struct *sensors_data)
{
	pthread_mutex_lock(&variables_mutex);
	sensors->Ta = sensors_data->Ta;
	sensors->T = sensors_data->T;
	sensors->Ti = sensors_data->Ti;
	sensors->H = sensors_data->H;
	sensors->No = sensors_data->No;
	pthread_mutex_unlock(&variables_mutex);
}

void updateActuatorsTemperatureGlobalVar(struct actuators_struct *actuators, struct actuators_struct *actuators_data)
{
	pthread_mutex_lock(&variables_mutex);
	actuators->Q = actuators_data->Q;
	actuators->Na = actuators_data->Na;
	actuators->Ni = actuators_data->Ni;
	pthread_mutex_unlock(&variables_mutex);
}

void updateActuatorsHeightGlobalVar(struct actuators_struct *actuators, struct actuators_struct *actuators_data)
{
	pthread_mutex_lock(&variables_mutex);
	actuators->Na = actuators_data->Na;
	actuators->Ni = actuators_data->Ni;
	actuators->Nf = actuators_data->Nf;
	pthread_mutex_unlock(&variables_mutex);
}

void updateReferenceGlobalVar(struct reference_struct *reference, struct reference_struct *reference_data)
{
	pthread_mutex_lock(&variables_mutex);
	reference->T_ref = reference_data->T_ref;
	reference->H_ref = reference_data->H_ref;
	pthread_mutex_unlock(&variables_mutex);
}

void copyReference(struct reference_struct *reference_data, struct reference_struct *reference)
{
	pthread_mutex_lock(&variables_mutex);
	reference_data->T_ref = reference->T_ref;
	reference_data->H_ref = reference->H_ref;
	pthread_mutex_unlock(&variables_mutex);
};

void copyAllData(struct sensors_struct *sensors,      struct actuators_struct *actuators,      struct reference_struct *reference,
				 struct sensors_struct *sensors_data, struct actuators_struct *actuators_data, struct reference_struct *reference_data)
{
	pthread_mutex_lock(&variables_mutex);
	sensors_data->Ta = sensors->Ta;
	sensors_data->T = sensors->T;
	sensors_data->Ti = sensors->Ti;
	sensors_data->H = sensors->H;
	sensors_data->No = sensors->No;

	actuators_data->Q =  actuators->Q;
	actuators_data->Ni = actuators->Ni;
	actuators_data->Na = actuators->Na;
	actuators_data->Nf = actuators->Nf;

	reference_data->T_ref = reference->T_ref;
	reference_data->H_ref = reference->H_ref;
	pthread_mutex_unlock(&variables_mutex);
}