// Include libraries
#include "../include/main.h"

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


/* Main function */
int main(int argc, char *argv[])
{
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
	pthread_t t1, t2, t3, t4, t5, t6, t7;

	// Creating threads
	pthread_create(&t1, NULL, (void *) printSensorData, NULL);
	pthread_create(&t2, NULL, (void *) temperatureController, NULL);
	pthread_create(&t3, NULL, (void *) heightController, NULL);
	pthread_create(&t4, NULL, (void *) getReferenceValues, NULL);
	pthread_create(&t5, NULL, (void *) temperatureAlarm, NULL);
	pthread_create(&t6, NULL, (void *) sendToBuffer, NULL);
	pthread_create(&t7, NULL, (void *) writeIntoFile, NULL);

	// Joining threads
	pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    pthread_join(t6, NULL);
    pthread_join(t7, NULL);
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
void temperatureController(){

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
		// lock socket
		pthread_mutex_lock(&socket_mutex);
		Ta = read_sensor("sta0", local_socket, dest_address);
		T = read_sensor("st-0", local_socket, dest_address);
		Ti = read_sensor("sti0", local_socket, dest_address);
		No = read_sensor("sno0", local_socket, dest_address);
		H = read_sensor("sh-0", local_socket, dest_address);
		// unlock socket
		pthread_mutex_unlock(&socket_mutex);

		// Controller
		const float Kp_T = 4.0;				// proportional gain
		float output_T = Kp_T*(T_ref - T);

		// Defining aux variables so it doesn't have to deal with protected variables
		float aux_Q;
		float aux_Na;
		float aux_Ni;
		
		// Calculating capacitance
		C = S*P*B*H;

		// Calculating actuators values
		if (output_T > 0.0){
			aux_Na = 0.0;
			aux_Ni = 0.0;
			aux_Q = output_T*C - Ni*S*(Ti - T) - Na*S*(80-T) - (T-Ta)/R;
			if (aux_Q >= 1000000.0){
				aux_Q = 1000000.0;
				aux_Na = (output_T*C - Ni*S*(Ti - T) - aux_Q - (T-Ta)/R)/(S*(80-T));
				if (aux_Na >= 10.0){
					aux_Na = 10.0;
					aux_Ni = (output_T*C - aux_Na*S*(80-T) - (T-Ta)/R - aux_Q)/(S*(Ti - T));
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
			aux_Ni = (output_T*C - Na*S*(80-T) - (T-Ta)/R - Q)/(S*(Ti - T));
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
		// lock socket
		pthread_mutex_lock(&socket_mutex);
		Q = actuate("aq-\0", aux_Q, local_socket, dest_address);
		Na = actuate("ana\0", aux_Na, local_socket, dest_address);
		Ni = actuate("ani\0", aux_Ni, local_socket, dest_address);
		// unlock socket
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
void heightController(){

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
		// lock socket
		pthread_mutex_lock(&socket_mutex);
		No = read_sensor("sno0", local_socket, dest_address);
		H = read_sensor("sh-0", local_socket, dest_address);
		// unlock socket
		pthread_mutex_unlock(&socket_mutex);

		// Controller
    	const float Kp_H = 3;				// proportional gain
		float output_H = Kp_H*(H_ref - H);

		// Defining aux variables so it doesn't have to deal with protected variables
		float aux_Ni;
		float aux_Na;
		float aux_Nf;

		// Calculating actuators values
		if (output_H > 0){
			aux_Na = 0;
			aux_Nf = 0;
			aux_Ni = output_H*B*P - Na + No + Nf;
			if (aux_Ni >= 100.0){
				aux_Ni = 100.0;
				aux_Na = output_H*B*P - aux_Ni + No + Nf;
				if (aux_Na >= 10.0)
					aux_Na = 10.0;
			}
		}else if (output_H < 0){
			aux_Ni = 0;
			aux_Na = 0;
			aux_Nf = -output_H*B*P + Ni + Na + No;
			if (aux_Nf >= 100.0)
				aux_Nf = 100.0;
		}else{
			aux_Ni = 0;
			aux_Na = 0;
			aux_Nf = 0;
		}

		// Sendint actuators commands to the boiler and assigning it to global variables
		// lock socket
		pthread_mutex_lock(&socket_mutex);
		Ni = actuate("ani\0", aux_Ni, local_socket, dest_address);
		Na = actuate("ana\0", aux_Na, local_socket, dest_address);
		Nf = actuate("anf\0", aux_Nf, local_socket, dest_address);
		// unlock socket
		pthread_mutex_unlock(&socket_mutex);

		// Updating timespec
		tp_H.tv_nsec += periodo_ns_H;

		while (tp_H.tv_nsec >= nsec_per_sec) {
			tp_H.tv_nsec -= nsec_per_sec;
			tp_H.tv_sec++;
		}
	}
}


// Let the user enter the desired reference values
void getReferenceValues(){

	while(1){
		if (getchar() == '\n') {
			// lock console
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("Informe a temperatura de referencia (ºC): \n");
			scanf("%f", &T_ref);
			printf("Informe o nível de referência (m): \n");
			scanf("%f", &H_ref);
			// unlock console
			pthread_mutex_unlock(&console_mutex);
			getchar();
			sleep(1);
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
		//lock socket
		pthread_mutex_lock(&socket_mutex);
		T = read_sensor("st-0", local_socket, dest_address);
		pthread_mutex_unlock(&socket_mutex);
		//unlock socket

		if(T >= 30.0){
			// print alarm
			// lock console
			pthread_mutex_lock(&console_mutex);
			printf("\e[1;1H\e[2J");
			printf("\n\n\n\n\n");
			printf("---------                 !!! ALARM !!!                 ---------\n");
			printf("---------    Current Temperature is higher than 30ºC    ---------\n");
			printf("---------    Current Temperature is %f           ---------\n", T);
			printf("\n\n\n\n\n");
			// unlock console
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


// Send values to double buffer approx one time per second
void sendToBuffer(){
	
	//Defining current time in seconds
	time_t seconds;

	while(1){
		// Get current time in seconds
   		seconds = time(NULL);

		insertInBuffer(seconds, T, T_ref, H, H_ref, Q, Ta, Ti, Ni, No, Nf, Na);
		
		sleep(1);
	}
}

//Write values into log file
void writeIntoFile(){

	// variables related to the file
	FILE *fp;
	char filename[14] = "./log/log.csv";

	//write colunm names
	fp = fopen(filename, "w");
	fprintf(fp,"Data (DD//MM/AAAA), Hora (HH:MM:SS), T (ºC), T_ref (ºC), H (m), H_ref (m), Q (J/s), Ta (ºC), Ti (ºC), Ni (m3/s), No (m3/s), Nf (m3/s), Na (m3/s)\n");
	fclose(fp);

	// defining some variables which will be used in the while loop
	double *buffer;

	//variables used to convert seconds into current time
	struct timespec tp_B;
	struct tm *tm;

	//char array which will receive date and time string
	char date_and_time[26]; 

	while(1){
		// Receive buffer
		buffer = waitFullBuffer();

		// Write values into file
		fp = fopen(filename, "a");

		for (int i=0; i<(BUFFER_SIZE/N_VARIABLES); i++){

			// write date and time
			tp_B.tv_sec = buffer[N_VARIABLES*i];
			tm = localtime(&tp_B.tv_sec);
			strftime(date_and_time, 26, "%d-%m-%Y, %H:%M:%S", tm);
			fprintf(fp, "%s", date_and_time);

			// write the other values
			for (int j=1; j<(N_VARIABLES); j++)
				fprintf(fp, ", %f", buffer[j+i*N_VARIABLES]);

			//break line
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}