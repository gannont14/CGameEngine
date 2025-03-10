#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "raylib.h"
#include"types.h"
#include"player.h"
#include<netinet/in.h>


#define PORT 5000
#define MAX_PLAYERS 4

// Client --> Server Structs
typedef struct{

  char player_name[32];

} ConnectionRequest;

typedef struct{

  u8 input;
  Camera3D camera;

} PlayerInput;

// Server --> Client Structs
typedef struct{

  u8 player_count;
  Player players[MAX_PLAYERS];

} GameState;

typedef enum{

  CONNECTION_SUCCESS = 0x0,
  ERROR_UNKNOWN      =  0x1,
  ERROR_GAMEFULL     =  0x2,

} ResponseMessage;

typedef struct{

  ResponseMessage response_message;

} ConnectionResponse;

typedef struct {

  u8 num_players;
  u8 player_id;

} GameStartInfo;


typedef enum{

  CONNECTION_REQUEST  =  0x1,
  CONNECTION_RESPONSE =  0x2,
  PLAYER_INPUT        = 0x3,
  GAME_STATE          = 0x4,
  GAME_START          = 0x5,

} PacketType;

// Packet Struct, might separate this into client and server packet
typedef struct{

  PacketType type; 
  u8 seq;
  u8 client;
  union {
    ConnectionRequest connection_request;
    ConnectionResponse connection_response;
    PlayerInput player_input;
    GameState game_state; // bottlenecking the size of the union
    GameStartInfo game_start_info;
  };

}Packet;

void print_packet_information(Packet* packet);

#endif // !TRANSPORT_H


