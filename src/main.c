// Include libraries
#include "../include/main.h"

// Initializing Time constants
const long int nsec_per_sec = 1000000000L; 		// The number of nsecs per sec.

// Initializing sensors variables
struct sensors_struct sensors = {0.0, 0.0, 0.0, 0.0, 0.0};

// Initializing Actuators variables
struct actuators_struct actuators = {0.0, 0.0, 0.0, 0.0};

// Initializing reference values
struct reference_struct reference = {20.0, 2.0};

// Initializing const variables
const float R = 0.001;                     		// Resistência térmica do isolamento (2mm madeira) [0.001 Grau / (Joule/segundo)]
const float B = 4.0;                       		// Área da base do recipiente [4 m2]
const float P = 1000.0;                    		// Peso específico da água [1000 Kg/m3]
const float S = 4184.0;                    		// Calor específico da água [4184 Joule/Kg.Celsius]


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
	clock_gettime(CLOCK_MONOTONIC, &t);

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


/*  Print the sensor data into the console approx one time per second using sleep */
void printSensorData()
{
	// Initializing aux structs so it doesn't have to deal with protected variables
	struct sensors_struct sensors_data = {0.0, 0.0, 0.0, 0.0, 0.0};
	struct actuators_struct actuators_data = {0.0, 0.0, 0.0, 0.0};
	struct reference_struct reference_data = {0.0, 0.0};

	while(1){
		// copy all data to aux structs
		copyAllData(&sensors, &actuators, &reference, &sensors_data, &actuators_data, &reference_data);
		// print data into the console
		consoleData(&sensors_data, &actuators_data, &reference_data);
		sleep(1);
	}
}


/* Temperature proportional controller with period of 50ms*/
void temperatureController()
{
	// Initializing controller period
	long int period_ns_T = 50000000;

	// Defining and initializing controller timespec
	struct timespec tp_T;
	tp_T.tv_sec = t.tv_sec + 1;
	tp_T.tv_nsec = t.tv_nsec;

	// Initializing aux structs so it doesn't have to deal with protected variables
	struct sensors_struct sensors_data = {0.0, 0.0, 0.0, 0.0, 0.0};
	struct actuators_struct actuators_data = {0.0, 0.0, 0.0, 0.0};
	struct reference_struct reference_data = {0.0, 0.0};

	// Define C variable for controller equations
	float C; 

	// Variables for controller
	const float Kp_T = 4.0;				// proportional gain
	float output_T;
	
	while(1){

		// Get current time
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_T, NULL);

		// copy reference data into aux struct
		copyReference(&reference_data, &reference);

		// Read sensor data and put it into the aux struct
		getSensorData(&sensors_data);
		// Update global sensor struct
		updateSensorsGlobalVar(&sensors, &sensors_data);

		// gain*error
		output_T = Kp_T*(reference_data.T_ref - sensors_data.T);

		// Calculating capacitance
		C = S*P*B*sensors_data.H;

		// Calculating actuators values
		if (output_T > 0.0){
			actuators_data.Na = 0.0;
			actuators_data.Ni = 0.0;
			actuators_data.Q = output_T*C - actuators_data.Ni*S*(sensors_data.Ti - sensors_data.T) - actuators_data.Na*S*(80-sensors_data.T) - (sensors_data.T-sensors_data.Ta)/R;
			if (actuators_data.Q >= 1000000.0){
				actuators_data.Q = 1000000.0;
				actuators_data.Na = (output_T*C - actuators_data.Ni*S*(sensors_data.Ti - sensors_data.T) - actuators_data.Q - (sensors_data.T-sensors_data.Ta)/R)/(S*(80-sensors_data.T));
				if (actuators_data.Na >= 10.0){
					actuators_data.Na = 10.0;
					actuators_data.Ni = (output_T*C - actuators_data.Na*S*(80-sensors_data.T) - (sensors_data.T-sensors_data.Ta)/R - actuators_data.Q)/(S*(sensors_data.Ti - sensors_data.T));
					if (actuators_data.Ni >= 100.0){
						actuators_data.Ni = 100.0;
					} else if(actuators_data.Ni <= 0.0){
						actuators_data.Ni = 0.0;
					}
				}
			}
		}else if (output_T < 0.0){
			actuators_data.Q = 0.0;
			actuators_data.Na = 0.0;
			actuators_data.Ni = (output_T*C - actuators_data.Na*S*(80-sensors_data.T) - (sensors_data.T-sensors_data.Ta)/R - actuators_data.Q)/(S*(sensors_data.Ti - sensors_data.T));
			if (actuators_data.Ni >= 100.0){
				actuators_data.Ni = 100.0;
			} else if(actuators_data.Ni <= 0.0){
				actuators_data.Ni = 0.0;
			}
		}else{
			actuators_data.Q = 0.0;
			actuators_data.Na = 0.0;
			actuators_data.Ni = 0.0;
		}
		
		// send actuation command to the boiler
		setTemperatureActuators(&actuators_data);
		// update actuator global struct
		updateActuatorsTemperatureGlobalVar(&actuators, &actuators_data);

		// Updating timespec
		tp_T.tv_nsec += period_ns_T;

		while (tp_T.tv_nsec >= nsec_per_sec) {
			tp_T.tv_nsec -= nsec_per_sec;
			tp_T.tv_sec++;
		}
	}
}


/* Height proportional controller with period of 70ms*/
void heightController()
{
	// Initializing controller period
	long int period_ns_H = 70000000;

	// Defining and initializing controller timespec
	struct timespec tp_H;
	tp_H.tv_sec = t.tv_sec + 1;
	tp_H.tv_nsec = t.tv_nsec;

	// Initializing aux variables so it doesn't have to deal with protected variables
	struct sensors_struct sensors_data = {0.0, 0.0, 0.0, 0.0, 0.0};
	struct actuators_struct actuators_data = {0.0, 0.0, 0.0, 0.0};
	struct reference_struct reference_data = {0.0, 0.0};

	// Varibles for controller
    const float Kp_H = 4.0;
	float output_H;

	while(1){

		// Get current time
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_H, NULL);

		// Read sensor data and put it into the aux struct
		getSensorData(&sensors_data);
		// Update global sensor struct
		updateSensorsGlobalVar(&sensors, &sensors_data);

		/* Controller */
		// copy reference data into aux struct
		copyReference(&reference_data, &reference);

		// gain*error
		output_H = Kp_H*(reference_data.H_ref - sensors_data.H);

		// Calculating actuators values
		if (output_H > 0){
			actuators_data.Na = 0;
			actuators_data.Nf = 0;
			actuators_data.Ni = output_H*B*P - actuators_data.Na + sensors_data.No + actuators_data.Nf;
			if (actuators_data.Ni >= 100.0){
				actuators_data.Ni = 100.0;
				actuators_data.Na = output_H*B*P - actuators_data.Ni + sensors_data.No + actuators_data.Nf;
				if (actuators_data.Na >= 10.0)
					actuators_data.Na = 10.0;
			}
		}else if (output_H < 0){
			actuators_data.Ni = 0;
			actuators_data.Na = 0;
			actuators_data.Nf = -output_H*B*P + actuators_data.Ni + actuators_data.Na + sensors_data.No;
			if (actuators_data.Nf >= 100.0)
				actuators_data.Nf = 100.0;
		}else{
			actuators_data.Ni = 0;
			actuators_data.Na = 0;
			actuators_data.Nf = 0;
		}

		// send actuation command to the boiler
		setHeightActuators(&actuators_data);
		// update actuator global struct
		updateActuatorsHeightGlobalVar(&actuators, &actuators_data);

		// Updating timespec
		tp_H.tv_nsec += period_ns_H;

		while (tp_H.tv_nsec >= nsec_per_sec) {
			tp_H.tv_nsec -= nsec_per_sec;
			tp_H.tv_sec++;
		}
	}
}


/* Let the user enter the desired reference values */
void getReferenceValues()
{
	// Initializing aux struct so it doesn't have to deal with protected variables
	struct reference_struct reference_data = {0.0, 0.0};

	while(1){
		if (getchar() == '\n') {

			// read referece values from keyboard
			consoleGetReferenceValues(&reference_data);
			// update actuator global struct
			updateReferenceGlobalVar(&reference, &reference_data);	

			getchar();
			sleep(1);
		}
    }
}


/* Alarm for temperature higher than 30ºC with period of 10ms */
void temperatureAlarm()
{
	// Initializing controller period
	long int period_ns_A = 10000000;

	// Defining and initializing controller timespec
	struct timespec tp_A;
	tp_A.tv_sec = t.tv_sec + 1;
	tp_A.tv_nsec = t.tv_nsec;

	// Initializing aux struct so it doesn't have to deal with protected variables
	struct sensors_struct sensors_data = {0.0, 0.0, 0.0, 0.0, 0.0};

	while(1){
		//get current time
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp_A, NULL);

		// reading sensor data
		getSensorData(&sensors_data);
		// updating global sensor struct
		updateSensorsGlobalVar(&sensors, &sensors_data);

		if(sensors_data.T >= 30.0){
			// print alarm
			consoleAlarm(sensors_data.T);
		};
	
		// Updating timespec
		tp_A.tv_nsec += period_ns_A;

		while (tp_A.tv_nsec >= nsec_per_sec) {
			tp_A.tv_nsec -= nsec_per_sec;
			tp_A.tv_sec++;
		}		
	}
}


/* Send values to double buffer approx one time per second */
void sendToBuffer()
{
	//Defining current time in seconds
	time_t seconds;

	// Initializing aux struct so it doesn't have to deal with protected variables
	struct sensors_struct sensors_data = {0.0, 0.0, 0.0, 0.0, 0.0};
	struct actuators_struct actuators_data = {0.0, 0.0, 0.0, 0.0};
	struct reference_struct reference_data = {0.0, 0.0};

	while(1){
		// Get current time in seconds
   		seconds = time(NULL);
		
		// copy all data to aux structs
		copyAllData(&sensors, &actuators, &reference, &sensors_data, &actuators_data, &reference_data);
		// insert data values in buffer
		insertInBuffer(seconds, &sensors_data, &actuators_data, &reference_data);
		
		sleep(1);
	}
}


/* Write values into log file */
void writeIntoFile()
{
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