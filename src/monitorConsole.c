#include "../include/monitorConsole.h"

pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

void consoleData(struct sensors_struct *sensors_data, struct actuators_struct *actuators_data, struct reference_struct *reference_data)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("Sensores\n");
	printf("\tTa: %f (ºC)\n", sensors_data->Ta);
	printf("\tT: %f (ºC)\n", sensors_data->T);
	printf("\tTi: %f (ºC)\n", sensors_data->Ti);
	printf("\tNo: %f (kg/s)\n", sensors_data->No);
	printf("\tH: %f (m)\n\n", sensors_data->H);
	printf("Atuadores\n");
	printf("\tQ: %f (J/s)\n", actuators_data->Q);
	printf("\tNi: %f (kg/s)\n", actuators_data->Ni);
	printf("\tNa: %f (kg/s)\n", actuators_data->Na);
	printf("\tNf: %f (kg/s)\n\n", actuators_data->Nf);
	printf("Referência\n");
	printf("\tT_ref: %f (ºC)\n", reference_data->T_ref);
	printf("\tH_ref: %f (m)\n\n", reference_data->H_ref);
	printf("Para alterar os valores de referência aperte ENTER\n");
	pthread_mutex_unlock(&console_mutex);
}

void consoleGetReferenceValues(struct reference_struct *reference_data)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");

	do{
	printf("Informe a temperatura de referência (ºC): \n");
	scanf("%f", &reference_data->T_ref);
	if (reference_data->T_ref > 30.0)
		printf("A temperatura não pode ser maior que 30ºC\n\n");
	} while(reference_data->T_ref > 30.0);

	do{
	printf("\nInforme o nível de referência (m): \n");
	scanf("%f", &reference_data->H_ref);
	if (reference_data->H_ref > 3.0 || reference_data->H_ref < 0.1)
		printf("O nível da água deve estar entre 0.1m e 3m para não danificar a caldeira\n\n");
	} while(reference_data->H_ref > 3.0 || reference_data->H_ref < 0.1);

	pthread_mutex_unlock(&console_mutex);
}

void consoleTemperatureAlarm(double T)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("\n\n\n\n\n");
	printf("---------                 !!! ALARM !!!            ---------\n");
	printf("---------    Temperatura atual é maior que 30ºC    ---------\n");
	printf("---------    Temperatura atual é %f ºC   ---------\n", T);
	printf("\n\n\n\n\n");
	pthread_mutex_unlock(&console_mutex);
}

void consoleHeightAlarm(double H)
{
	pthread_mutex_lock(&console_mutex);
	printf("\e[1;1H\e[2J");
	printf("\n\n\n\n\n");
	printf("---------                 !!! ALARM !!!              ---------\n");
	if (H > 3.0){
	printf("---------    O nível da água atual é maior que 3m    ---------\n");
	printf("---------    O nível da água atual é %f m      ---------\n", H);
	} else if (H < 0.1){
		printf("---------    O nível da água atual é menor que 0.1m  ---------\n");
		printf("---------    O nível da água atual é %f m      ---------\n", H);
	}
	pthread_mutex_unlock(&console_mutex);
}

