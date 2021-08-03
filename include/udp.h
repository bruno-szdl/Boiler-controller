#ifndef UDP_H
#define UDP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>

#define FAILURE 1

int create_local_socket(void);                                                  // Create local socket
struct sockaddr_in create_dest_address(char *dest, int dest_port);              // Create destination addres
void send_msg(int local_socket, struct sockaddr_in dest_address, char *msg);    // Send msg to server
int get_msg(int local_socket, char *buffer, int buffer_size);                   // Receive msg from server

#endif