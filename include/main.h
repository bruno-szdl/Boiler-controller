#include <time.h>
#include <unistd.h>

#include "variables.h"
#include "monitorSocket.h"
#include "monitorVariables.h"
#include "monitorBuffer.h"
#include "monitorConsole.h"

//Defining Time constants
const long int nsec_per_sec;        /* The number of nsecs per sec. */
const long int usec_per_sec;        /* The number of usecs per sec. */
const int nsec_per_usec;            /* The number of nsecs per usec. */

//Defining ensors variables
float Ta;                           // Outter temperature [ºC]
float T;                            // Water temperature [ºC]
float Ti;                           // Temperature of water entering the boiler [º]
float No;                           // Water flow leaving the boiler [m3/s]
float H;                            // Height of water column [m]

//Defining Actuators variables
float Q;                            // Heat flux [J/s]
float Ni;                           // Water flow entering the boiler [º]
float Na;                           // water flow entering the boiler at 80ºC [ºC]
float Nf;                           // water leaving the boiler to reservatory [º]

//Defining reference values
float T_ref; //
float H_ref;

//Defining const variables
const float R;                      // isolation thermal resistance (2mm wood) [0.001 ºC / (J/s)]
const float B;                      // boiler's base area [4 m2]
const float P;                      // water density [1000 kg/m3]
const float S;                      // water Cp [4184 J/kg.ºC]

//Defining mutexes
pthread_mutex_t socket_mutex; 
pthread_mutex_t console_mutex;

//Other variabless
float C;                            // Capacitance of the water inside the boiler [J/ºC]
float V;                            // Water volume inside the boiler[m3]

struct timespec t;                  // Current time

int local_socket;
struct sockaddr_in dest_address;

void getReferenceValues();          // Temperature for entering reference values by console
void temperatureController();       // Temperature controller
void heightController();            // Height controller
void printSensorData();             // Print sensor data on console
void temperatureAlarm();            // Temperature alarm
void sendToBuffer();                // Sends values to buffer
void writeIntoFile();               // Write values into file