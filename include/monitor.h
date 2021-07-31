#ifndef MONITOR
#define MONITOR

#include <pthread.h>
#include <stdio.h>

#define BUFFER_SIZE 360
#define N_VARIABLES 12

void insertInBuffer(double p0, double p1, double p2, double p3, double p4, double p5, double p6,
					double p7, double p8, double p9, double p10, double p11);
double *waitFullBuffer();

#endif