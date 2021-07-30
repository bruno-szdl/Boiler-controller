#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//sensors variables
struct params_struct
{
    float Ta;
    float T;
    float Ti;
    float No;
    float H;
    float Q;
    float Ni;
    float Na;
    float Nf;
};

float T_ref;		                       // temperatura da água no interior do recipiente desejada [Grau Celsius]
float H_ref;                               // altura da coluna de água dentro do recipiente desejado [m]

//Other variables
float Qt;                                  // fluxo de calor total fornecido à água do recipiente [Joule/segundo]
float Qo;                                  // fluxo de calor retirado pela água quente que deixa o recipiente [Joule/segundo]
float Qi;                                  // fluxo de calor inserido pela água fria que entra no recipiente [Joule/segundo]
float Qe;                                  // fluxo de calor através do isolamento do recipiente [Joule/segundo]
float C;                                   // capacitância térmica da água no recipiente [Joule/Celsius]
float V;                                   // volume de água dentro do recipiente [m3]

struct timespec t;                         //Hora atual

int local_socket;
struct sockaddr_in dest_address;

void getReferenceValues();
void temperatureController();
void heightController();
void printSensorData();
void temperatureAlarm();