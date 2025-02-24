#include "server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_socklen_t.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "transport.h"
	
#define MAXLINE 1024
	

ClientInfo connected_clients[MAX_CLIENTS];
int  num_clients_connected;

int sockfd;
struct sockaddr_in servaddr;
GameStatus game_status;

void server_receive_connection_requests(void);
void broadcast_packet_to_clients(Packet* packet);
void server_broadcast_game_start(void);
void* server_create_receiving_thread(void* args);
int send_packet(Packet* packet, struct sockaddr_in* cliaddr);
int try_append_new_client(struct sockaddr_in client_addr);
ClientInfo generate_client_info(struct sockaddr_in client_addr);
void send_client_connection_response(int client_id, int status, struct sockaddr_in* cliaddrs);

int init_server(char* host) {
  printf("Hosting server on : %s", host);

  // set status to game loading
  game_status = GAME_LOADING;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

  printf("Server: successfully created address and sockets\n");


  pthread_t receiving_thread_id;
  int res = pthread_create(&receiving_thread_id,
                           NULL,
                           server_create_receiving_thread,
                           NULL);
  if(res == -1)
  {
    printf("Error creating receiving thread\n");
    return -1;
  }

  printf("Server: Created receiving thread\n");
  int game_started = 0;
  while(1)
  {
    //disgustin
    if(game_started == 0 && num_clients_connected == 2)
    {
      printf("BROADCASTING GAME START\n");
      game_started = 1;
      server_broadcast_game_start();
    }
    continue;
  }

  // clent up all of the threads
  pthread_join(receiving_thread_id, NULL);
		
	return 0;
}

void* server_create_receiving_thread(void* args)
{
  (void)args; // tells comp and vim I'm not using, no more warning
  // main loop
  while(1)
  {
    // call correct function depending on current game state
    switch (game_status) {
      case GAME_LOADING:
        server_receive_connection_requests();
        break;
      case GAME_ONGOING:
        break;
      case GAME_ENDING:
        break;
      default:
        printf("Invalid game status");
    }

  }
  return NULL;
}

void server_receive_connection_requests(void)
{
  printf("Server: Call to handle connection request packet---------\n");
  struct sockaddr_in cliaddr;
	Packet* packet;
  packet = malloc(sizeof(Packet));

	memset(&cliaddr, 0, sizeof(cliaddr));

  int n;
  socklen_t len;
	len = sizeof(cliaddr); //len is value/result
	
	n = recvfrom(sockfd, (Packet *)packet, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
  if(n == -1)
  {
    printf("Server: Error receiving packet\n");
  }

  print_packet_information(packet);

  // append new client
  int status = try_append_new_client(cliaddr);
  // send response back to client that they have been accepted
  send_client_connection_response(num_clients_connected, status, &cliaddr);
  printf("Server: Call to handle connection request packet complete\n");
  printf("Server: Amount of clients now connected: %d\n", num_clients_connected);
}

void server_broadcast_game_start(void)
{

  for(int i = 0; i < num_clients_connected; i++)
  {
    Packet* packet = malloc(sizeof(Packet));

    packet->seq = 0;
    packet->type = GAME_START;
    packet->client = i;
    packet->game_start_info.num_players = num_clients_connected;
    packet->game_start_info.player_id = i;

    send_packet(packet, &connected_clients[i].client_addr);
    printf("Sent GAME_START packet to %d\n", i);
    free(packet);
  }

}


void send_client_connection_response(int client_id, int status, struct sockaddr_in* cliaddr)
{
  Packet* packet;
  packet = malloc(sizeof(Packet));

  packet->type = CONNECTION_RESPONSE;
  packet->seq = 0;
  packet->client = client_id;
  packet->connection_response.response_message = status;

  send_packet(packet, cliaddr);

  free(packet);

  printf("Server: connection response packet freed\n");
}

int send_packet(Packet* packet, struct sockaddr_in* cliaddr)
{
  int len = sizeof(*cliaddr);
  int val;
	val =  sendto(sockfd, packet, sizeof(Packet),
		0, (const struct sockaddr *) cliaddr,
			len);
  printf("Server: Properly sent, returning\n");

  return val;
}

int try_append_new_client(struct sockaddr_in client_addr)
{
  if(num_clients_connected < MAX_PLAYERS)
  {
    connected_clients[num_clients_connected] = generate_client_info(client_addr);
    num_clients_connected++;
    return CONNECTION_SUCCESS;
  }
  else {
    return ERROR_GAMEFULL;
  }
}

ClientInfo generate_client_info(struct sockaddr_in client_addr)
{
  // hard code client id for now, shouldn't need to change, might not be used
  u8 client_id = num_clients_connected + 1;
  return (ClientInfo){
    client_addr,
    client_id
  };
}

