// Include libraries
#include "../include/main.h"
#include "../include/udp.h"
#include "../include/boilerCommunication.h"

// Initializing Time constants
const long int nsec_per_sec = 1000000000L; /* The number of nsecs per sec. */
const long int usec_per_sec = 1000000L;    /* The number of usecs per sec. */
const int nsec_per_usec = 1000;            /* The number of nsecs per usec. */

// Initializing ensors variables
float Ta = 0;
float T = 0;
float Ti = 0;
float No = 0;
float H = 0;

// Initializing Actuators variables
float Q = 0;
float Ni = 0;
float Na = 0;
float Nf = 0;

// Initializing reference values
float T_ref = 20;
float H_ref = 2;

// Initializing const variables
const float R = 0.001;                     // Resistência térmica do isolamento (2mm madeira) [0.001 Grau / (Joule/segundo)]
const float B = 4.0;                       // Área da base do recipiente [4 m2]
const float P = 1000.0;                    // Peso específico da água [1000 Kg/m3]
const float S = 4184.0;                    // Calor específico da água [4184 Joule/Kg.Celsius]

// Initializing mutexes
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;


// Main function
int main(int argc, char *argv[]){

	// Check if args are properly set
	if (argc < 3) { 
		fprintf(stderr,"Uso: udpcliente endereço porta \n");
		fprintf(stderr,"onde o endereço é o endereço do servidor \n");
		fprintf(stderr,"porta é o número da porta do servidor \n");
		fprintf(stderr,"palavra é a palavra que será enviada ao servidor \n");
		fprintf(stderr,"exemplo de uso:\n");
		fprintf(stderr,"   udpcliente baker.das.ufsc.br 1234 \"ola\"\n");
		exit(FAILURE);
	}

	// Get local socket and dest address
	int dest_port = atoi(argv[2]);
	local_socket = create_local_socket();
	dest_address = create_dest_address(argv[1], dest_port);

	// Get current time
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Assure that the boiler simulation has begun before creating threads 
	printf("\e[1;1H\e[2J");
	printf(" Aperte em '''Simula''' no simulador da caldeira,\n");
	printf(" apenas então aperte ENTER no terminal.\n\n");
	getchar();

	// Defining threads
	pthread_t t1, t2, t3, t4, t5;

	// Creating threads
	pthread_create(&t1, NULL, (void *) getReferenceValues, NULL);
	pthread_create(&t2, NULL, (void *) printSensorData, NULL);
	pthread_create(&t3, NULL, (void *) temperatureController, NULL);
	pthread_create(&t4, NULL, (void *) heightController, NULL);
	pthread_create(&t5, NULL, (void *) temperatureAlarm, NULL);

	// Joining threads
	pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
}


// Let the user enter the desired reference values
void getReferenceValues()
{
	while(1){
		if (getchar() == '\n') {
			// lock
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("Informe a temperatura de referencia (ºC): \n");
			scanf("%f", &T_ref);
			printf("Informe o nível de referência (m): \n");
			scanf("%f", &H_ref);
			// unlock
			pthread_mutex_unlock(&console_mutex);
			getchar();
			sleep(1);
		}
    }
}


/*  Print the sensor data into the console
approx one time per second using sleep */
void printSensorData(){

	while(1){
		// lock
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
		// unlock
		pthread_mutex_unlock(&console_mutex);

		sleep(1);
	}
}


/* Temperature proportional controller
with period of 50ms*/
void temperatureController()
{

	// Initializing controller period
	long int periodo_ns_T = 50000000;
	float periodo_s_T = 0.05;

	// Defining and initializing controller timespec
	struct timespec tp_T;
	tp_T.tv_sec = t.tv_sec + 1;
	tp_T.tv_nsec = t.tv_nsec;

	while(1){
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_T, NULL);
		// reading sensors values and assign it to the global variables
		// lock
		pthread_mutex_lock(&socket_mutex);
		Ta = read_sensor("sta0", local_socket, dest_address);
		T = read_sensor("st-0", local_socket, dest_address);
		Ti = read_sensor("sti0", local_socket, dest_address);
		No = read_sensor("sno0", local_socket, dest_address);
		H = read_sensor("sh-0", local_socket, dest_address);
		// unlock
		pthread_mutex_unlock(&socket_mutex);

		// Controller
		const float Kp_T = 5.0;				// proportional gain
		float output_T = Kp_T*(T_ref - T);

		// Defining aux variables so it doesn't have to deal with protected variables
		float aux_Q;
		float aux_Na;
		float aux_Ni;
		
		// Calculating capacitance
		C = S*P*B*H;

		// Calculating actuators values
		if (output_T > 0.0){
			aux_Q = output_T*C - Ni*S*(Ti - T) - Na*S*(80-T) - (T-Ta)/R;
			aux_Na = 0.0;
			aux_Ni = 0.0;
			if (aux_Q >= 1000000.0){
				aux_Q = 1000000.0;
				aux_Na = (output_T*C - aux_Ni*S*(Ti - T) - aux_Q - (T-Ta)/R)/(S*(80-T));
				if (aux_Na >= 10.0){
					aux_Na = 10.0;
					aux_Ni = (output_T*C - Na*S*(80-T) - (T-Ta)/R - aux_Q)/(S*(Ti - T));
					if (aux_Ni >= 100.0){
						aux_Ni = 100.0;
					} else if(aux_Ni <= 0.0){
						aux_Ni = 0.0;
					}
				}
			}
		}else if (output_T < 0.0){
			aux_Q = 0.0;
			aux_Na = 0.0;
			aux_Ni = (output_T*C - Na*S*(80-T) - (T-Ta)/R - aux_Q)/(S*(Ti - T));
			if (aux_Ni >= 100.0){
				aux_Ni = 100.0;
			} else if(aux_Ni <= 0.0){
				aux_Ni = 0.0;
			}
		}else{
			aux_Q = 0.0;
			aux_Na = 0.0;
			aux_Ni = 0.0;
		}
		
		// Sendint actuators commands to the boiler and assigning it to global variables
		// lock
		pthread_mutex_lock(&socket_mutex);
		Q = actuate("aq-\0", aux_Q, local_socket, dest_address);
		Na = actuate("ana\0", aux_Na, local_socket, dest_address);
		Ni = actuate("ani\0", aux_Ni, local_socket, dest_address);
		// unlock
		pthread_mutex_unlock(&socket_mutex);

		// Updating timespec
		tp_T.tv_nsec += periodo_ns_T;

		while (tp_T.tv_nsec >= nsec_per_sec) {
			tp_T.tv_nsec -= nsec_per_sec;
			tp_T.tv_sec++;
		}
	}
}


/* Height proportional controller
with period of 70ms*/
void heightController()
{

	// Initializing controller period
	long int periodo_ns_H = 70000000;
	float periodo_s_H = 0.07;

	// Defining and initializing controller timespec
	struct timespec tp_H;
	tp_H.tv_sec = t.tv_sec + 1;
	tp_H.tv_nsec = t.tv_nsec;

	while(1){
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_H, NULL);
		// reading sensors values and assign it to the global variables
		// lock
		pthread_mutex_lock(&socket_mutex);
		No = read_sensor("sno0", local_socket, dest_address);
		H = read_sensor("sh-0", local_socket, dest_address);
		// unlock
		pthread_mutex_unlock(&socket_mutex);

		// Controller
    	const float Kp_H = 5;				// proportional gain
		float output_H = Kp_H*(H_ref - H);

		// Defining aux variables so it doesn't have to deal with protected variables
		float aux_Ni;
		float aux_Na;
		float aux_Nf;

		// Calculating actuators values
		if (output_H > 0){
			aux_Ni = output_H*B*P - Na + No + Nf;
			aux_Na = 0;
			aux_Nf = 0;
			if (aux_Ni >= 100.0){
				aux_Ni = 100.0;
				aux_Na = output_H*B*P - aux_Ni + No + aux_Nf;
				if (aux_Na >= 10.0)
					aux_Na = 10.0;
			}
		}else if (output_H < 0){
			aux_Ni = 0;
			aux_Na = 0;
			aux_Nf = -output_H*B*P + aux_Ni + Na + No;
			if (aux_Nf >= 100.0)
				aux_Nf = 100.0;
		}else{
			aux_Ni = 0;
			aux_Na = 0;
			aux_Nf = 0;
		}

		// Sendint actuators commands to the boiler and assigning it to global variables
		// lock
		pthread_mutex_lock(&socket_mutex);
		Ni = actuate("ani\0", aux_Ni, local_socket, dest_address);
		Na = actuate("ana\0", aux_Na, local_socket, dest_address);
		Nf = actuate("anf\0", aux_Nf, local_socket, dest_address);
		// unlock
		pthread_mutex_unlock(&socket_mutex);

		// Updating timespec
		tp_H.tv_nsec += periodo_ns_H;

		while (tp_H.tv_nsec >= nsec_per_sec) {
			tp_H.tv_nsec -= nsec_per_sec;
			tp_H.tv_sec++;
		}
	}
}


/* Alarm for temperature higher than 30ºC
with period of 10ms */
void temperatureAlarm()
{
	// Initializing controller period
	long int periodo_ns_A = 10000000;
	float periodo_s_A = 0.01;

	// Defining and initializing controller timespec
	struct timespec tp_A;
	tp_A.tv_sec = t.tv_sec + 1;
	tp_A.tv_nsec = t.tv_nsec;

	while(1){
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_A, NULL);

		// reading temperature
		//lock
		pthread_mutex_lock(&socket_mutex);
		T = read_sensor("st-0", local_socket, dest_address);
		pthread_mutex_unlock(&socket_mutex);
		//unlock

		if(T >= 30.0){
			// print alarm
			// lock
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("\n\n\n\n\n");
			printf("---------                 !!! ALARM !!!                 ---------\n");
			printf("---------    Current Temperature is higher than 30ºC    ---------\n");
			printf("---------    Current Temperature is %f           ---------\n", T);
			printf("\n\n\n\n\n");
			// unlock
			pthread_mutex_unlock(&console_mutex);
		};
	
		// Updating timespec
		tp_A.tv_nsec += periodo_ns_A;

		while (tp_A.tv_nsec >= nsec_per_sec) {
			tp_A.tv_nsec -= nsec_per_sec;
			tp_A.tv_sec++;
		}		
	}
}
