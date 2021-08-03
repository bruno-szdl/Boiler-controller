#ifndef VARIABLES
#define VARIABLES

#include "../include/udp.h"

//Defining sensors variables
struct sensors_struct{
    float Ta;                           // Outter temperature [ºC]
    float T;                            // Water temperature [ºC]
    float Ti;                           // Temperature of water entering the boiler [º]
    float No;                           // Water flow leaving the boiler [m3/s]
    float H;                            // Height of water column [m]
};

//Defining Actuators variables
struct actuators_struct{
    float Q;                            // Heat flux [J/s]
    float Ni;                           // Water flow entering the boiler [º]
    float Na;                           // water flow entering the boiler at 80ºC [ºC]
    float Nf;                           // water leaving the boiler to reservatory [º]
};

//Defining reference values
struct reference_struct{
    float T_ref;
    float H_ref;
};

int local_socket;
struct sockaddr_in dest_address;

#endif