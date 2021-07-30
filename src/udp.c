#include "../include/udp.h"

int create_local_socket(void)
{
	int local_socket;		/* Socket usado na comunicacão */

	local_socket = socket( PF_INET, SOCK_DGRAM, 0);
	if (local_socket < 0) {
		perror("socket");
		return -1;
	}
	return local_socket;
}

struct sockaddr_in create_dest_address(char *dest, int dest_port)
{
	struct sockaddr_in server; 	/* Endereço do server incluindo ip e porta */
	struct hostent *dest_internet;	/* Endereço dest em formato próprio */
	struct in_addr dest_ip;		/* Endereço dest em formato ip numérico */

	if (inet_aton ( dest, &dest_ip ))
		dest_internet = gethostbyaddr((char *)&dest_ip, sizeof(dest_ip), AF_INET);
	else
		dest_internet = gethostbyname(dest);

	if (dest_internet == NULL) {
		fprintf(stderr,"Endereço de rede inválido\n");
		exit(FAILURE);
	}

	memset((char *) &server, 0, sizeof(server));
	memcpy(&server.sin_addr, dest_internet->h_addr_list[0], sizeof(server.sin_addr));
	server.sin_family = AF_INET;
	server.sin_port = htons(dest_port);

	return server;
}

void send_msg(int local_socket, struct sockaddr_in dest_address, char *msg)
{
	/* Envia msg ao server */

	if (sendto(local_socket, msg, strlen(msg)+1, 0, (struct sockaddr *) &dest_address, sizeof(dest_address)) < 0 )
	{ 
		perror("sendto");
		return;
	}
}


int get_msg(int local_socket, char *buffer, int BUFFER_SIZE)
{
	int bytes_recebidos;		/* Número de bytes recebidos */

	/* Espera pela msg de resposta do server */
	bytes_recebidos = recvfrom(local_socket, buffer, BUFFER_SIZE, 0, NULL, 0);
	if (bytes_recebidos < 0)
	{
		perror("recvfrom");
	}

	return bytes_recebidos;
}