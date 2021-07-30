#include "../include/udp.h"


// Create local socket
int create_local_socket(void){
	
	int local_socket;		// Socket to be used in communication

	local_socket = socket( PF_INET, SOCK_DGRAM, 0);
	if (local_socket < 0) {
		perror("socket");
		return -1;
	}
	return local_socket;
}


// Create destination address
struct sockaddr_in create_dest_address(char *dest, int dest_port)
{
	struct sockaddr_in server; 		// Server address (IP and port)
	struct hostent *dest_internet; 
	struct in_addr dest_ip;			// Destination address in numeric ip format 

	if (inet_aton ( dest, &dest_ip ))
		dest_internet = gethostbyaddr((char *)&dest_ip, sizeof(dest_ip), AF_INET);
	else
		dest_internet = gethostbyname(dest);

	if (dest_internet == NULL){
		fprintf(stderr,"Endereço de rede inválido\n");
		exit(FAILURE);
	}

	memset((char *) &server, 0, sizeof(server));
	memcpy(&server.sin_addr, dest_internet->h_addr_list[0], sizeof(server.sin_addr));
	server.sin_family = AF_INET;
	server.sin_port = htons(dest_port);

	return server;
}


// Send msg to server
void send_msg(int local_socket, struct sockaddr_in dest_address, char *msg){

	if (sendto(local_socket, msg, strlen(msg)+1, 0, (struct sockaddr *) &dest_address, sizeof(dest_address)) < 0 ){ 
		perror("sendto");
		return;
	}
}


// Receive msg from server
int get_msg(int local_socket, char *buffer, int BUFFER_SIZE)
{
	int received_bytes;

	// Wait for server's response
	received_bytes = recvfrom(local_socket, buffer, BUFFER_SIZE, 0, NULL, 0);
	if (received_bytes < 0){
		perror("recvfrom");
	}

	return received_bytes;
}