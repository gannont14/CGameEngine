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
int seqnum = 0;

pthread_mutex_t game_state_mutex;
const int POLLING_RATE_HZ = 60;

void send_connection_request_packet(void);
u8 get_current_client_input_state(void);
void* client_broadcast_thread(void* arg);
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
      break;
    }
    continue;
  }

  // init the palyer's broadcast thread before creating their client
  pthread_t client_broadcast_thread_id;
  pthread_create(&client_broadcast_thread_id,
                 NULL,
                 &client_broadcast_thread,
                 NULL);
  printf("Client %d broadcast thread created\n", client_id);

  usleep(100000);
  if(players == NULL)
  {
    printf("Init client when players wasn't malloced\n");
    return 1;
  }

  init_player(client_id);


  // clean up threads
  pthread_join(client_receiving_thread_id, NULL);
  pthread_join(client_broadcast_thread_id, NULL);
	close(sockfd);
  pthread_mutex_destroy(&game_state_mutex);

	return 0;
}

void* client_broadcast_thread(void* arg)
{
  (void)arg;

  const int POLLING_RATE_SLEEP_TIME = 1000 / POLLING_RATE_HZ;

  while(1)
  {

    u8 client_input = get_current_client_input_state();

    pthread_mutex_lock(&game_state_mutex);

    Packet packet = {
      .type = PLAYER_INPUT,
      .seq = ++seqnum % MAX_SEQ_NUM, 
      .client = client_id,
      .player_input = (PlayerInput){
        client_input,
        camera
      }
    };

    /*printf("Camera Target: [%f, %f, %f]\n",*/
    /*       camera.target.x,*/
    /*       camera.target.y,*/
    /*       camera.target.z*/
    /*       );*/


    pthread_mutex_unlock(&game_state_mutex);

    sendto(sockfd, &packet, sizeof(Packet),
      0, (const struct sockaddr *) &servaddr,
        sizeof(servaddr));

    if(client_input != 0)
    {
      /*printf("Client%d: sending inp: %d\n", client_id, client_input);*/
    }
    usleep(POLLING_RATE_SLEEP_TIME);
  }


  return NULL;
}

u8 get_current_client_input_state(void)
{

    u8 inp = 0;

    if(IsKeyDown(KEY_W)) { inp += PLAYER_FORWARD; }
    if(IsKeyDown(KEY_A)) { inp += PLAYER_LEFT; }
    if(IsKeyDown(KEY_S)) { inp += PLAYER_BACKWARD; }
    if(IsKeyDown(KEY_D)) { inp += PLAYER_RIGHT; }
    if(IsKeyDown(KEY_SPACE)) {inp += PLAYER_UP; }
    if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_C)) { inp += PLAYER_DOWN; }

    return inp;

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
    /*printf("Client: %d Packet of size %d received back from server\n", packet->type, n);*/

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
  printf("Client: number of players: %d\n", packet.game_start_info.num_players);
  num_players = packet.game_start_info.num_players;
  players = malloc(sizeof(Player) * num_players);
  for(int i = 0; i < num_players; i++)
  {
      players[i].client_id = i + 1;
      players[i].pos = (Vector3){0.0f, 0.0f, 0.0f};
      
      // Initialize camera with safe defaults
      players[i].camera.position = (Vector3){0.0f, 0.0f, 0.0f};
      players[i].camera.target = (Vector3){0.0f, 0.0f, 0.0f};
      players[i].camera.up = (Vector3){0.0f, 1.0f, 0.0f};
      players[i].camera.fovy = PLAYER_FOV;
      players[i].camera.projection = CAMERA_PERSPECTIVE;
      
      printf("Initialized player %d in local array\n", players[i].client_id);
  }
}

void client_handle_game_state(Packet packet)
{
  // lock mutex
  pthread_mutex_lock(&game_state_mutex);

  for(int i = 0; i < num_players; i++)
  {
    Player* remote_player = &packet.game_state.players[i];

    if(remote_player->client_id  != client_id)
    {
      players[remote_player->client_id - 1].pos = remote_player->pos;
      players[remote_player->client_id - 1].camera = remote_player->camera;

      /*printf("Updated remote player %d position \n", client_id);*/
      /*printf("remote player pos: [%.2f, %.2f, %.2f] \n",*/
      /*       remote_player->pos.x,*/
      /*       remote_player->pos.y,*/
      /*       remote_player->pos.z);*/
      /*printf("Remote players local pos: [%.2f, %.2f, %.2f]\n",*/
      /*       players[remote_player->client_id - 1].pos.x,*/
      /*       players[remote_player->client_id - 1].pos.y,*/
      /*       players[remote_player->client_id - 1].pos.z);*/
      // update with other filds for rotation and other stats
    }
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
  packet->seq = ++seqnum % MAX_SEQ_NUM;
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
