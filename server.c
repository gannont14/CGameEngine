#include "server.h"
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
#include "player.h"
#include "raylib.h"
#include "constants.h"
#include "raymath.h"
#include "transport.h"
	
#define MAXLINE 1024
	

ClientInfo connected_clients[MAX_CLIENTS];
int  num_clients_connected;
Player* server_players;

int sockfd;
struct sockaddr_in servaddr;
GameStatus game_status;


const int TICK_RATE_MS = 1000 / TICK_RATE;

void server_receive_connection_requests(void);
void* server_game_thread(void* args);
void server_broadcast_game_state(void);
void server_receive_player_input(void);
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
    if(game_started == 0 && num_clients_connected == NUM_PLAYERS_TO_START_SERVER)
    {
      /*printf("BROADCASTING GAME START\n");*/
      game_started = 1;
      server_broadcast_game_start();
      break;
    }
    continue;
  }

  pthread_t broadcast_thread_id;
  res = pthread_create(&broadcast_thread_id,
                           NULL,
                           &server_game_thread,
                           NULL);


  if(res == -1)
  {
    printf("Error creating broadcast thread\n");
    return -1;
  }

  while(1)
  {
    continue;
  }

  // clent up all of the threads
  pthread_join(receiving_thread_id, NULL);
  pthread_join(broadcast_thread_id, NULL);
		
	return 0;
}

void* server_game_thread(void* args)
{
  (void)args;
  printf("Server: Creating server's broadcast thread\n");

  const int MAX_INPUT_PACKETS = 60;
  while(1)
  {
    for(int i = 0; i < MAX_INPUT_PACKETS; i++)
    {
      server_receive_player_input();
    }
    /*printf("Server: received player input\n");*/

    server_broadcast_game_state();
    /*printf("Server: broadcasted game state\n");*/

    usleep(TICK_RATE_MS * 1000);
  }

  return NULL;
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
        server_receive_player_input();
        break;
      case GAME_ENDING:
        break;
      default:
        printf("Invalid game status");
    }

  }
  return NULL;
}

void server_receive_player_input(void)
{
  struct sockaddr_in cliaddr;
  socklen_t len = sizeof(cliaddr); //len is value/result
  Packet* packet = malloc(sizeof(Packet));
	
	int n = recvfrom(sockfd, (Packet *)packet, MAXLINE,
				MSG_DONTWAIT, ( struct sockaddr *) &cliaddr, // using a non-waiting socket to it doesn't block
				&len);
  bool player_taken_action[num_players];
  for(int i = 0; i < num_players; i++)
  {
    player_taken_action[i] = 0;
  }

  if(n > 0)
  {
    if(packet->type == PLAYER_INPUT)
    {

      // find client id, this could be circumvented if the player's id is where they are in the list
      if(packet->player_input.input != 0){
        printf("packet from client|Client id: %d|seqnum: %d|Input: %d\n",
               packet->client,
               packet->seq,
               packet->player_input.input);
      }

      int client_index = -1;
      for(int i = 0; i < num_clients_connected; i++)
      {
        /*printf("Server_players client id: %d\n", server_players[i].client_id);*/
        if(server_players[i].client_id == packet->client)
        {
          /*printf("Found client index at: %d\n", i);*/
          client_index = i;
          break;
        }
      }
      if(client_index != -1 && player_taken_action[client_index] != true)
      {
        // lock mutex
        pthread_mutex_lock(&game_state_mutex);
        // handle the player's input 

        u8 input = packet->player_input.input;
        Player* player = &server_players[client_index];
        /*double move_speed = PLAYER_MOVE_SPEED / (double)TICK_RATE / SERVER_TO_PLAYER_MOVE_SPEED_SCALING;*/

        double seconds_per_tick = 1.0f / (double)TICK_RATE;
        double move_speed = SERVER_MOVE_SPEED * seconds_per_tick * 4.0f;
        /*double move_speed = PLAYER_MOVE_SPEED * seconds_per_tick * seconds_per_tick;*/


        player->camera = packet->player_input.camera;

        // these are ripped straight from the raylib source code  ( and don't work :D )
        // In my client side I am using updateCameraPro, which won't work all that well,
        // so I'm stealing this instead
        Vector3 movement = (Vector3){ 0.0f, 0.0f, 0.0f };


        // if, if, if , ififififi
        if((input & PLAYER_FORWARD) > 0)
        { 
          movement.x +=  move_speed;
        }
        if((input & PLAYER_BACKWARD) > 0)
        { 
          movement.x -= move_speed;
        }
        if((input & PLAYER_LEFT) > 0)
        { 
          movement.y -= move_speed;
        }
        if((input & PLAYER_RIGHT) > 0)
        { 
          movement.y += move_speed;
        }

        if((input & PLAYER_UP) > 0)
        { movement.z += move_speed; }
        if((input & PLAYER_DOWN) > 0)
        { movement.z -= move_speed; }


        Vector3 forward = Vector3Normalize(Vector3Subtract(player->camera.target, player->camera.position));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, Vector3Normalize(player->camera.up)));

        Vector3 forwardWorldPlane = forward;
        forwardWorldPlane.y = 0.0f; // Zero out the Y component
        forwardWorldPlane = Vector3Normalize(forwardWorldPlane); // Renormalize
        Vector3 offsetForward = Vector3Scale(forwardWorldPlane, movement.x);

        Vector3 rightWorldPlane = right;
        rightWorldPlane.y = 0.0f; 
        rightWorldPlane = Vector3Normalize(rightWorldPlane); 
        Vector3 offsetRight = Vector3Scale(rightWorldPlane, movement.y);

        Vector3 offsetUp = { 0.0f, movement.z, 0.0f };

        // Combine all movements
        Vector3 totalOffset = Vector3Add(Vector3Add(offsetForward, offsetRight), offsetUp);

        // DEBUG
        if(totalOffset.x != 0 || totalOffset.y != 0 || totalOffset.z != 0)
        {
          printf("SERVER MOVEMENT: dt=%.6f, raw_speed=%.6f, calculated_offset=[%.6f, %.6f, %.6f]\n", 
                 seconds_per_tick, move_speed, totalOffset.x, totalOffset.y, totalOffset.z);
        }

        // Apply the combined movement
        player->pos = Vector3Add(player->pos, totalOffset);

        // Update camera position to match new player position
        player->camera.position = player->pos;

        // Update camera target to maintain the same view direction
        player->camera.target = Vector3Add(player->camera.position, 
                                   Vector3Scale(forward, Vector3Distance(player->camera.position, 
                                                                         player->camera.target)));

        player_taken_action[player->client_id] = true;
        // unlock mutex
        pthread_mutex_unlock(&game_state_mutex);
      }
    }
  }
  free(packet);
}

void server_receive_connection_requests(void)
{
  /*printf("Server: Call to handle connection request packet---------\n");*/
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
  printf("Server: Amount of clients now connected: %d\n", num_clients_connected);
}

void server_broadcast_game_start(void)
{
  server_players = malloc(sizeof(Player) * num_clients_connected);

  // set all of the palyers client ids
  for(int i = 0; i < num_clients_connected; i++)
  {
    server_players[i].client_id = i + 1;
      // Initialize with default camera values
      server_players[i].pos = default_player_pos;      
      server_players[i].camera.position = server_players[i].pos;
      server_players[i].camera.target = default_camera_target;
      server_players[i].camera.up = default_camera_up;
      server_players[i].camera.fovy = PLAYER_FOV;
      server_players[i].camera.projection = CAMERA_PERSPECTIVE;
  }

  game_status = GAME_ONGOING;

  for(int i = 0; i < num_clients_connected; i++)
  {
    Packet* packet = malloc(sizeof(Packet));

    packet->seq = 0;
    packet->type = GAME_START;
    packet->client = 0;
    packet->game_start_info.num_players = num_clients_connected;
    packet->game_start_info.player_id = i;

    send_packet(packet, &connected_clients[i].client_addr);
    printf("Sent GAME_START packet to %d\n", i);
    free(packet);
  }

}


void server_broadcast_game_state(void)
{
  /*printf("Server: Broadcasting game state\n");*/

  Packet* packet = malloc(sizeof(Packet));

  packet->type = GAME_STATE;
  packet->seq = 0;
  packet->client = 0;

  pthread_mutex_lock(&game_state_mutex);

  packet->game_state.player_count = num_clients_connected;
  /*printf("Num clients connected : %d\n", num_clients_connected);*/
  for(int i = 0; i < num_clients_connected; i++)
  {
    packet->game_state.players[i] = server_players[i];
  }

  pthread_mutex_unlock(&game_state_mutex);

  // send to all 
  for(int i = 0; i < num_clients_connected; i++)
  {
    send_packet(packet, &connected_clients[i].client_addr);
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
}

int send_packet(Packet* packet, struct sockaddr_in* cliaddr)
{
  int len = sizeof(*cliaddr);
  int val;
	val =  sendto(sockfd, packet, sizeof(Packet),
		0, (const struct sockaddr *) cliaddr,
			len);

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

