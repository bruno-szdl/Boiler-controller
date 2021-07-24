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

float Q = 0;
float Ni = 0;
float Na = 0;
float Nf = 0;

float T_ref = 20;
float H_ref = 2;

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
		exit(FALHA);
	}

	int porta_destino = atoi( argv[2]);
	socket_local = cria_socket_local();
	endereco_destino = cria_endereco_destino(argv[1], porta_destino);

	sleep(1);

	clock_gettime(CLOCK_MONOTONIC ,&t);

	printf("\e[1;1H\e[2J");
	printf(" Aperte em '''Simula''' no simulador da caldeira,\n");
	printf(" apenas então aperte ENTER no terminal.\n\n");
	getchar();

	pthread_t t1, t2, t3, t4, t5;

	pthread_create(&t1, NULL, (void *) getReferenceValues, NULL);
	pthread_create(&t2, NULL, (void *) printSensorData, NULL);
	pthread_create(&t3, NULL, (void *) temperatureController, NULL);
	pthread_create(&t4, NULL, (void *) heightController, NULL);
	pthread_create(&t5, NULL, (void *) temperatureAlarm, NULL);

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

void temperatureController()
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
		Ta = read_sensor("sta0", socket_local, endereco_destino); //lê valor de Ta
		T = read_sensor("st-0", socket_local, endereco_destino); //lê valor de T
		Ti = read_sensor("sti0", socket_local, endereco_destino); //lê valor de Ti
		No = read_sensor("sno0", socket_local, endereco_destino); //lê valor de No
		H = read_sensor("sh-0", socket_local, endereco_destino); //lê valor de H
		pthread_mutex_unlock(&socket_mutex);

		// temperature
		float error_T = T_ref - T;
		integral_T += error_T*periodo_s_T;
		float output_T = Kp_T*error_T + Ki_T*integral_T;// + Kd_T*derivative_T;

		C = S*P*B*H;
		if (output_T > 0){
			Q = output_T*C - Ni*S*(Ti - T) - Na*S*(80-T) - (T-Ta)/R;
			Na = 0;
			if (Q >= 1000000.0){
				Q = 1000000.0;
				Na = (output_T*C - Ni*S*(Ti - T) - Q - (T-Ta)/R)/(S*(80-T));
				if (Na >= 10.0){
					Na = 10.0;
				}
			}
		}else if (output_T < 0){
			Q = 0;
			Na = 0;
		}else{
			Q = 0;
			Na = 0;
		};
		pthread_mutex_lock(&socket_mutex);
		send_message("aq-\0", Q, socket_local, endereco_destino);
		send_message("ana\0", Na, socket_local, endereco_destino);
		pthread_mutex_unlock(&socket_mutex);

		tp_T.tv_nsec += periodo_ns_T;

		while (tp_T.tv_nsec >= nsec_per_sec) {
			tp_T.tv_nsec -= nsec_per_sec;
			tp_T.tv_sec++;
		}
	}
};

void heightController()
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
		No = read_sensor("sno0", socket_local, endereco_destino); //lê valor de No
		H = read_sensor("sh-0", socket_local, endereco_destino); //lê valor de H
		pthread_mutex_unlock(&socket_mutex);

		// temperature
		float error_H = H_ref - H;
		integral_H += error_H*periodo_s_H;
		float output_H = Kp_H*error_H + Ki_H*integral_H;// + Kd_H*derivative_H;

		if (output_H > 0){
			Nf = 0;
			Ni = output_H*B*P - Na + No + Nf;
			if (Ni >= 100.0)
				Ni = 100.0;
		}else if (output_H < 0){
			Ni = 0;
			Nf = -output_H*B*P + Ni + Na + No;
			if (Nf >= 100.0)
				Nf = 100.0;
		}else{
			Ni = 0;
			Nf = 0;
		};
		pthread_mutex_lock(&socket_mutex);
		send_message("ani\0", Ni, socket_local, endereco_destino);
		send_message("anf\0", Nf, socket_local, endereco_destino);
		pthread_mutex_unlock(&socket_mutex);

		tp_H.tv_nsec += periodo_ns_H;

		while (tp_H.tv_nsec >= nsec_per_sec) {
			tp_H.tv_nsec -= nsec_per_sec;
			tp_H.tv_sec++;
		}
	}
};

void printSensorData()
{
	for (int i = 0; i < 3000; i++){
		pthread_mutex_lock(&console_mutex);
		printf("\e[1;1H\e[2J");
		printf("Sensores\n");
		printf("\tValor de Ta: %f (ºC)\n", Ta);
		printf("\tValor de T: %f (ºC)\n", T);
		printf("\tValor de Ti: %f (ºC)\n", Ti);
		printf("\tValor de No: %f (kg/s)\n", No);
		printf("\tValor de H: %f (m)\n\n", H);
		printf("Atuadores\n");
		printf("\tValor de Q: %f (J/s)\n", Q);
		printf("\tValor de Ni: %f (kg/s)\n", Ni);
		printf("\tValor de Na: %f (kg/s)\n", Na);
		printf("\tValor de Nf: %f (kg/s)\n\n", Nf);
		printf("Os valores atuais de referência são\n");
		printf("\tT_ref: %f (ºC)\n", T_ref);
		printf("\tH_ref: %f (m)\n\n", H_ref);
		printf("Para alterar os valores aperte ENTER\n");
		pthread_mutex_unlock(&console_mutex);

		sleep(1);
	};
};

void temperatureAlarm()
{
	int periodo_ns_A = 10000000;
	float periodo_s_A = 0.01;

	struct timespec tp_A;
	tp_A.tv_sec = t.tv_sec + 1;
	tp_A.tv_nsec = t.tv_nsec;

	for (int i = 0; i < 30000; i++)
	{
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_A, NULL);

		if(T >= 30.0){
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("\n\n\n\n\n");
			printf("---------                 !!! ALARM !!!                 ---------\n");
			printf("---------    Current Temperature is higher than 30ºC    ---------\n");
			printf("---------    Current Temperature is %f           ---------\n", T);
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
