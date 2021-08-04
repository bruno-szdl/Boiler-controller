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

//Defining const variables
const float R;                      // isolation thermal resistance (2mm wood) [0.001 ºC / (J/s)]
const float B;                      // boiler's base area [4 m2]
const float P;                      // water density [1000 kg/m3]
const float S;                      // water Cp [4184 J/kg.ºC]

//Other variables
struct timespec t;                  // Current time

//Functions prototypes
void getReferenceValues();          // Temperature for entering reference values by console
void temperatureController();       // Temperature controller
void heightController();            // Height controller
void printSensorData();             // Print sensor data on console
void temperatureAlarm();            // Temperature alarm
void heightAlarm();                 // Height alarm
void sendToBuffer();                // Sends values to buffer
void writeIntoFile();               // Write values into file