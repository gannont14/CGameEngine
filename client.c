#include <pthread.h>
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
#include "player.h"


struct sockaddr_in servaddr;
int sockfd;
int client_id = 0;
int client_running = 0;

pthread_mutex_t game_state_mutex;

void send_connection_request_packet(void);
void client_handle_game_state(Packet packet);
void client_handle_game_start(Packet packet);
void client_handle_connection_response(Packet packet);
void* client_receiving_thread(void* arg);

int init_client(void) {

  // creating mutexes ( mutecies? )
  pthread_mutex_init(&game_state_mutex, NULL);
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
		

  send_connection_request_packet();
  printf("Client has sent packet to join the server\n");

  // create receiving thread for the client
  pthread_t client_receiving_thread_id;
  pthread_create(&client_receiving_thread_id,
                 NULL, 
                 client_receiving_thread,
                 NULL);

  int game_started = 0; // flag so only runs once
  while(1)
  {
    // this is disgusting to look at but workaround because raylib only draws on main thread
    if(game_started == 0 && client_running == 1)
    {
      game_started = 1;
      init_player();
    }
    continue;
  }


  pthread_join(client_receiving_thread_id, NULL);
	close(sockfd);
  pthread_mutex_destroy(&game_state_mutex);

	return 0;
}

void* client_receiving_thread(void* arg)
{
  (void)arg;

	int n;
  socklen_t len;

  while(1)
  {
    Packet* packet = malloc(sizeof(Packet));
    len = sizeof(servaddr);
    n = recvfrom(sockfd, packet, sizeof(Packet),
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);
    if(n == -1)
    {
      printf("Error receiving packet from server");
    }
    printf("Client: Packet of size %d received back from client\n", n);

    switch (packet->type) {
      case CONNECTION_RESPONSE:
        client_handle_connection_response(*packet);
        break;
      case GAME_START:
        client_handle_game_start(*packet);
        break;
      case GAME_STATE:
        client_handle_game_state(*packet);
        break;
      case CONNECTION_REQUEST: break;   // compiler wont shut up without, only server needs
      case PLAYER_INPUT: break;         // compiler wont shut up without, only server needs
    }
  }


  return NULL;
}

void client_handle_connection_response(Packet packet)
{
  // set client id 
  client_id = packet.client;
  print_packet_information(&packet);
}

void client_handle_game_start(Packet packet)
{
  // init the client's window, draw the worlds, malloc space for the player structs
  client_running = 1;
  players = malloc(sizeof(Player) * packet.game_start_info.num_players);
  num_players = packet.game_start_info.num_players;
}
void client_handle_game_state(Packet packet)
{
  // lock mutex
  pthread_mutex_lock(&game_state_mutex);


  for(int i = 0; i < num_players; i++)
  {
    Player* remote_player = &packet.game_state.players[i];

    int remote_player_id = i + 1;
    players[remote_player_id].pos = remote_player->pos;
    /*if(remote_player_id  != client_id)*/
    /*{*/
    /*  // update with other filds for rotation and other stats*/
    /*}*/
  }

  // unlock mutex
  pthread_mutex_unlock(&game_state_mutex);

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
