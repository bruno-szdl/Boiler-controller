#include "../include/monitorBuffer.h"

static double buffer_0[BUFFER_SIZE];
static double buffer_1[BUFFER_SIZE];

static int inuse = 0;
static int next_insert = 0;
static int save = -1;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t full_buffer_cond = PTHREAD_COND_INITIALIZER;

void insertInBuffer(double seconds, struct sensors_struct *sensors, struct actuators_struct *actuators, struct reference_struct *reference)
{
	pthread_mutex_lock(&buffer_mutex);
	if(inuse == 0){
		buffer_0[next_insert] = seconds;
		buffer_0[next_insert+1] = sensors->T;
		buffer_0[next_insert+2] = reference->T_ref;
		buffer_0[next_insert+3] = sensors->H;
		buffer_0[next_insert+4] = reference->H_ref;
		buffer_0[next_insert+5] = actuators->Q;
		buffer_0[next_insert+6] = sensors->Ta;
		buffer_0[next_insert+7] = sensors->Ti;
		buffer_0[next_insert+8] = actuators->Ni;
		buffer_0[next_insert+9] = sensors->No;
		buffer_0[next_insert+10] = actuators->Nf;
		buffer_0[next_insert+11] = actuators->Na;
	} else{
		buffer_1[next_insert] = seconds;
		buffer_1[next_insert+1] = sensors->T;
		buffer_1[next_insert+2] = reference->T_ref;
		buffer_1[next_insert+3] = sensors->H;
		buffer_1[next_insert+4] = reference->H_ref;
		buffer_1[next_insert+5] = actuators->Q;
		buffer_1[next_insert+6] = sensors->Ta;
		buffer_1[next_insert+7] = sensors->Ti;
		buffer_1[next_insert+8] = actuators->Ni;
		buffer_1[next_insert+9] = sensors->No;
		buffer_1[next_insert+10] = actuators->Nf;
		buffer_1[next_insert+11] = actuators->Na;
	}
		
	next_insert = next_insert + N_VARIABLES;

	if(next_insert == BUFFER_SIZE){
		save = inuse;
		inuse = (inuse+1) % 2;
		next_insert = 0;
		pthread_cond_signal(&full_buffer_cond);
	}
	pthread_mutex_unlock(&buffer_mutex);
}

double *waitFullBuffer()
{
	double *buffer = NULL;
	pthread_mutex_lock(&buffer_mutex);
	while(save == -1)
		pthread_cond_wait(&full_buffer_cond, &buffer_mutex);
		
	if(save == 0){
		buffer = buffer_0;
	} else{
		buffer = buffer_1;
	}

	save = -1;
	pthread_mutex_unlock(&buffer_mutex);

	return buffer;
}
