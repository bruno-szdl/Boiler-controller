#include "../include/monitor.h"

static double buffer_0[BUFFER_SIZE];
static double buffer_1[BUFFER_SIZE];

static int inuse = 0;
static int next_insert = 0;
static int save = -1;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t full_buffer_cond = PTHREAD_COND_INITIALIZER;

void insertInBuffer(double p0, double p1, double p2, double p3, double p4, double p5, double p6,
					double p7, double p8, double p9, double p10, double p11){

	//lock buffer
	pthread_mutex_lock(&buffer_mutex);
	if(inuse == 0){
		buffer_0[next_insert] = p0;
		buffer_0[next_insert+1] = p1;
		buffer_0[next_insert+2] = p2;
		buffer_0[next_insert+3] = p3;
		buffer_0[next_insert+4] = p4;
		buffer_0[next_insert+5] = p5;
		buffer_0[next_insert+6] = p6;
		buffer_0[next_insert+7] = p7;
		buffer_0[next_insert+8] = p8;
		buffer_0[next_insert+9] = p9;
		buffer_0[next_insert+10] = p10;
		buffer_0[next_insert+11] = p11;
	}
	else{
		buffer_1[next_insert] = p0;
		buffer_1[next_insert+1] = p1;
		buffer_1[next_insert+2] = p2;
		buffer_1[next_insert+3] = p3;
		buffer_1[next_insert+4] = p4;
		buffer_1[next_insert+5] = p5;
		buffer_1[next_insert+6] = p6;
		buffer_1[next_insert+7] = p7;
		buffer_1[next_insert+8] = p8;
		buffer_1[next_insert+9] = p9;
		buffer_1[next_insert+10] = p10;
		buffer_1[next_insert+11] = p11;
	}
		
	next_insert = next_insert + N_VARIABLES;

	if(next_insert == BUFFER_SIZE){
		save = inuse;
		inuse = (inuse+1) % 2;
		next_insert = 0;
		//signal
		pthread_cond_signal(&full_buffer_cond);
	}
	//unlock buffer
	pthread_mutex_unlock(&buffer_mutex);
}

double *waitFullBuffer(){

	double *buffer = NULL;
	//lock buffer
	pthread_mutex_lock(&buffer_mutex);
	while(save == -1)
		pthread_cond_wait(&full_buffer_cond, &buffer_mutex);
		
	if(save == 0){
		buffer = buffer_0;
	} else{
		buffer = buffer_1;
	}

	save = -1;
	//unlock buffer
	pthread_mutex_unlock(&buffer_mutex);

	return buffer;
}
