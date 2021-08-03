#include "../include/monitorConsole.h"

pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

void consoleData(struct sensors_struct *sensors_data, struct actuators_struct *actuators_data, struct reference_struct *reference_data)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("Sensores\n");
	printf("\tValor de Ta: %f (ºC)\n", sensors_data->Ta);
	printf("\tValor de T: %f (ºC)\n", sensors_data->T);
	printf("\tValor de Ti: %f (ºC)\n", sensors_data->Ti);
	printf("\tValor de No: %f (kg/s)\n", sensors_data->No);
	printf("\tValor de H: %f (m)\n\n", sensors_data->H);
	printf("Atuadores\n");
	printf("\tValor de Q: %f (J/s)\n", actuators_data->Q);
	printf("\tValor de Ni: %f (kg/s)\n", actuators_data->Ni);
	printf("\tValor de Na: %f (kg/s)\n", actuators_data->Na);
	printf("\tValor de Nf: %f (kg/s)\n\n", actuators_data->Nf);
	printf("Os valores atuais de referência são\n");
	printf("\tT_ref: %f (ºC)\n", reference_data->T_ref);
	printf("\tH_ref: %f (m)\n\n", reference_data->H_ref);
	printf("Para alterar os valores aperte ENTER\n");
	pthread_mutex_unlock(&console_mutex);
}

void consoleGetReferenceValues(struct reference_struct *reference_data)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("Informe a temperatura de referencia (ºC): \n");
	scanf("%f", &reference_data->T_ref);
	printf("Informe o nível de referência (m): \n");
	scanf("%f", &reference_data->H_ref);
	pthread_mutex_unlock(&console_mutex);
}

void consoleAlarm(double T)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("\n\n\n\n\n");
	printf("---------                 !!! ALARM !!!                 ---------\n");
	printf("---------    Current Temperature is higher than 30ºC    ---------\n");
	printf("---------    Current Temperature is %f           ---------\n", T);
	printf("\n\n\n\n\n");
	pthread_mutex_unlock(&console_mutex);
}

