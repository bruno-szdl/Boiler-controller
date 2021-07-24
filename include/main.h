#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


float Ta;
float T;
float Ti;
float No;
float H;

float Q;
float Ni;
float Na;
float Nf;


float T_ref;		                       // temperatura da água no interior do recipiente desejada [Grau Celsius]
float H_ref;                               // altura da coluna de água dentro do recipiente desejado [m]

//Define variables
float Q;                                   // fluxo de calor do elemento aquecedor [Joule/segundo]
float Qt;                                  // fluxo de calor total fornecido à água do recipiente [Joule/segundo]
float Qo;                                  // fluxo de calor retirado pela água quente que deixa o recipiente [Joule/segundo]
float Qi;                                  // fluxo de calor inserido pela água fria que entra no recipiente [Joule/segundo]
float Qe;                                  // fluxo de calor através do isolamento do recipiente [Joule/segundo]
float T;                                   // temperatura da água no interior do recipiente [Grau Celsius]
float Ti;                                  // temperatura da água que entra no recipiente [Grau Celsius]
float Ta;                                  // temperatura do ar ambiente em volta do recipiente [Grau Celsius]
float C;                                   // capacitância térmica da água no recipiente [Joule/Celsius]
float No;                                  // fluxo de água de saída do recipiente [Kg/segundo]
float Ni;                                  // fluxo de água de entrada do recipiente [Kg/segundo]
float Nf;                                  // fluxo de água de saída para esgoto controlada [Kg/segundo]
float Na;                                  // fluxo de água aquecida a 80C de entrada controlada [Kg/segundo]
float H;                                   // altura da coluna de água dentro do recipiente [m]
float V;                                   // volume de água dentro do recipiente [m3]

struct timespec t;                         //Hora atual

void getReferenceValues();
void temperatureController();
void heightController();
void printSensorData();
void temperatureAlarm();

int socket_local;
struct sockaddr_in endereco_destino;