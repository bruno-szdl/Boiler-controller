//Include libraries
#include "../include/main.h"
#include "../include/udp.h"
#include "../include/boilerCommunication.h"

//Define time constants
const long int nsec_per_sec = 1000000000L; /* The number of nsecs per sec. */
const long int usec_per_sec = 1000000L;    /* The number of usecs per sec. */
const int nsec_per_usec = 1000;            /* The number of nsecs per usec. */

//Define const variables
const float R = 0.001;                     // resistência térmica do isolamento (2mm madeira) [0.001 Grau / (Joule/segundo)]
const float B = 4.0;                       // área da base do recipiente [4 m2]
const float P = 1000.0;                    // peso específico da água [1000 Kg/m3]
const float S = 4184.0;                    // calor específico da água [4184 Joule/Kg.Celsius]

float T_ref = 20;
float H_ref = 2;

struct params_struct params;

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

//Main
int main(int argc, char *argv[])
{	
	if (argc < 3) { 
		fprintf(stderr,"Uso: udpcliente endereço porta \n");
		fprintf(stderr,"onde o endereço é o endereço do servidor \n");
		fprintf(stderr,"porta é o número da porta do servidor \n");
		fprintf(stderr,"palavra é a palavra que será enviada ao servidor \n");
		fprintf(stderr,"exemplo de uso:\n");
		fprintf(stderr,"   udpcliente baker.das.ufsc.br 1234 \"ola\"\n");
		exit(FAILURE);
	}

	int dest_port = atoi(argv[2]);
	local_socket = create_local_socket();
	dest_address = create_dest_address(argv[1], dest_port);

	sleep(1);

	clock_gettime(CLOCK_MONOTONIC ,&t);

	printf("\e[1;1H\e[2J");
	printf(" Aperte em '''Simula''' no simulador da caldeira,\n");
	printf(" apenas então aperte ENTER no terminal.\n\n");
	getchar();

	pthread_t t1, t2, t3, t4, t5;

	pthread_create(&t1, NULL, (void *) getReferenceValues, NULL);
	pthread_create(&t2, NULL, (void *) printSensorData, (void *) &params);
	pthread_create(&t3, NULL, (void *) temperatureController, (void *) &params);
	pthread_create(&t4, NULL, (void *) heightController, (void *) &params);
	pthread_create(&t5, NULL, (void *) temperatureAlarm, (void *) &params);

	pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
};

void getReferenceValues()
{
	while(1){
		if (getchar() == '\n') {
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("Informe a temperatura de referencia (ºC): \n");
			scanf("%f", &T_ref);
			printf("Informe o nível de referência (m): \n");
			scanf("%f", &H_ref);
			pthread_mutex_unlock(&console_mutex);
			getchar();
			sleep(1);
		}
    }
};

void printSensorData(struct params_struct *params)
{
	for (int i = 0; i < 3000; i++){
		pthread_mutex_lock(&console_mutex);
		printf("\e[1;1H\e[2J");
		printf("Sensores\n");
		printf("\tValor de Ta: %f (ºC)\n", params->Ta);
		printf("\tValor de T: %f (ºC)\n", params->T);
		printf("\tValor de Ti: %f (ºC)\n", params->Ti);
		printf("\tValor de No: %f (kg/s)\n", params->No);
		printf("\tValor de H: %f (m)\n\n", params->H);
		printf("Atuadores\n");
		printf("\tValor de Q: %f (J/s)\n", params->Q);
		printf("\tValor de Ni: %f (kg/s)\n", params->Ni);
		printf("\tValor de Na: %f (kg/s)\n", params->Na);
		printf("\tValor de Nf: %f (kg/s)\n\n", params->Nf);
		printf("Os valores atuais de referência são\n");
		printf("\tT_ref: %f (ºC)\n", T_ref);
		printf("\tH_ref: %f (m)\n\n", H_ref);
		printf("Para alterar os valores aperte ENTER\n");
		pthread_mutex_unlock(&console_mutex);

		sleep(1);
	};
};

void temperatureController(struct params_struct *params)
{
	const float Kp_T = 5.0;
    const float Ki_T = 0.0;
    float integral_T = 0.0;

	int periodo_ns_T = 50000000;
	float periodo_s_T = 0.05;

	struct timespec tp_T;
	tp_T.tv_sec = t.tv_sec + 1;
	tp_T.tv_nsec = t.tv_nsec;

	for (int i = 0; i < 30000; i++)
	{
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_T, NULL);
		
		pthread_mutex_lock(&socket_mutex);
		params->Ta = read_sensor("sta0", local_socket, dest_address); //lê valor de Ta
		params->T = read_sensor("st-0", local_socket, dest_address); //lê valor de T
		params->Ti = read_sensor("sti0", local_socket, dest_address); //lê valor de Ti
		params->No = read_sensor("sno0", local_socket, dest_address); //lê valor de No
		params->H = read_sensor("sh-0", local_socket, dest_address); //lê valor de H
		pthread_mutex_unlock(&socket_mutex);

		// temperature
		float error_T = T_ref - params->T;
		integral_T += error_T*periodo_s_T;
		float output_T = Kp_T*error_T + Ki_T*integral_T;// + Kd_T*derivative_T;

		C = S*P*B*params->H;
		if (output_T > 0){
			params->Q = output_T*C - params->Ni*S*(params->Ti - params->T) - params->Na*S*(80-params->T) - (params->T-params->Ta)/R;
			params->Na = 0;
			if (params->Q >= 1000000.0){
				params->Q = 1000000.0;
				params->Na = (output_T*C - params->Ni*S*(params->Ti - params->T) - params->Q - (params->T-params->Ta)/R)/(S*(80-params->T));
				if (params->Na >= 10.0){
					params->Na = 10.0;
				}
			}
		}else if (output_T < 0){
			params->Q = 0;
			params->Na = 0;
		}else{
			params->Q = 0;
			params->Na = 0;
		};
		pthread_mutex_lock(&socket_mutex);
		write_actuator("aq-\0", params->Q, local_socket, dest_address);
		write_actuator("ana\0", params->Na, local_socket, dest_address);
		pthread_mutex_unlock(&socket_mutex);

		tp_T.tv_nsec += periodo_ns_T;

		while (tp_T.tv_nsec >= nsec_per_sec) {
			tp_T.tv_nsec -= nsec_per_sec;
			tp_T.tv_sec++;
		}
	}
};

void heightController(struct params_struct *params)
{
    const float Kp_H = 5;
    const float Ki_H = 0.5;
    float integral_H = 0;

	int periodo_ns_H = 70000000;
	float periodo_s_H = 0.07;

	struct timespec tp_H;
	tp_H.tv_sec = t.tv_sec + 1;
	tp_H.tv_nsec = t.tv_nsec;

	for (int i = 0; i < 30000; i++)
	{
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_H, NULL);
		
		pthread_mutex_lock(&socket_mutex);
		params->No = read_sensor("sno0", local_socket, dest_address); //lê valor de No
		params->H = read_sensor("sh-0", local_socket, dest_address); //lê valor de H
		pthread_mutex_unlock(&socket_mutex);

		// temperature
		float error_H = H_ref - params->H;
		integral_H += error_H*periodo_s_H;
		float output_H = Kp_H*error_H + Ki_H*integral_H;// + Kd_H*derivative_H;

		if (output_H > 0){
			params->Nf = 0;
			params->Ni = output_H*B*P - params->Na + params->No + params->Nf;
			if (params->Ni >= 100.0)
				params->Ni = 100.0;
		}else if (output_H < 0){
			params->Ni = 0;
			params->Nf = -output_H*B*P + params->Ni + params->Na + params->No;
			if (params->Nf >= 100.0)
				params->Nf = 100.0;
		}else{
			params->Ni = 0;
			params->Nf = 0;
		};
		pthread_mutex_lock(&socket_mutex);
		write_actuator("ani\0", params->Ni, local_socket, dest_address);
		write_actuator("anf\0", params->Nf, local_socket, dest_address);
		pthread_mutex_unlock(&socket_mutex);

		tp_H.tv_nsec += periodo_ns_H;

		while (tp_H.tv_nsec >= nsec_per_sec) {
			tp_H.tv_nsec -= nsec_per_sec;
			tp_H.tv_sec++;
		}
	}
};

void temperatureAlarm(struct params_struct *params)
{
	int periodo_ns_A = 10000000;
	float periodo_s_A = 0.01;

	struct timespec tp_A;
	tp_A.tv_sec = t.tv_sec + 1;
	tp_A.tv_nsec = t.tv_nsec;

	for (int i = 0; i < 30000; i++)
	{
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_A, NULL);

		if(params->T >= 30.0){
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("\n\n\n\n\n");
			printf("---------                 !!! ALARM !!!                 ---------\n");
			printf("---------    Current Temperature is higher than 30ºC    ---------\n");
			printf("---------    Current Temperature is %f           ---------\n", params->T);
			printf("\n\n\n\n\n");
			pthread_mutex_unlock(&console_mutex);
		};
	
		tp_A.tv_nsec += periodo_ns_A;

		while (tp_A.tv_nsec >= nsec_per_sec) {
			tp_A.tv_nsec -= nsec_per_sec;
			tp_A.tv_sec++;
		};		
	};
};
