#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "client.h"
#include "transport.h"


struct sockaddr_in servaddr;
int sockfd;
int client_id = 0;

void send_connection_request_packet(void);

int init_client(void) {
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
		
	int n;
  socklen_t len;

  send_connection_request_packet();

  Packet* packet = malloc(sizeof(Packet));
  printf("Client: successfully malloced packet\n");

  len = sizeof(servaddr);
	n = recvfrom(sockfd, packet, sizeof(Packet),
				MSG_WAITALL, (struct sockaddr *) &servaddr,
				&len);

  if(n == -1)
  {
    printf("Error receiving connection response from server");
    return 1;
  }

  // set client id 
  client_id = packet->client;

  printf("Client: Packet of size %d received back from client\n", n);
  print_packet_information(packet);

	close(sockfd);
	return 0;
}

void send_connection_request_packet(void)
{
  Packet* packet;
  packet = malloc(sizeof(Packet));
  printf("Packet Malloced\n");
  const char* player_name = "Test Player";
  int name_len;

  packet->type = CONNECTION_REQUEST;
  packet->seq = 0;
  packet->client = client_id; // default to 0, anything above 1 has been set by the server
  name_len = sizeof(packet->connection_request.player_name) - 1;
  strncpy(packet->connection_request.player_name,
          player_name,
          name_len);
  // very roundabout way to null terminate the player name string
  packet->connection_request.player_name[name_len] = '\0';

	sendto(sockfd, packet, sizeof(Packet),
		0, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));

  printf("Client Request sent\n");
  free(packet);
}
