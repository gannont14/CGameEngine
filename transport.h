#ifndef TRANSPORT_H
#define TRANSPORT_H

#include"types.h"
#include"player.h"
#include<netinet/in.h>


#define PORT 5000
#define MAX_PLAYERS 4

// Client --> Server Structs
typedef struct{

  char player_name[32];
  struct sockaddr_in client_addr;
  u8 client_id;

} ConnectionRequest;

typedef struct{

  u8 input;

} PlayerInput;

// Server --> Client Structs
typedef struct{

  u8 player_count;
  Player players[MAX_PLAYERS];

} GameState;

typedef enum{

  ERROR_UNKNOWN  =  0x0,
  ERROR_GAMEFULL =  0x1,

} ResponseErrorMessage;

typedef struct{

  u8                    status;
  ResponseErrorMessage  message;
  u16                   client_id;

} ConnectionResponse;



typedef enum{

  CONNECTION_REQUEST  =  0x1,
  CONNECTION_RESPONSE =  0x2,
  PLAYER_INPUT        = 0x3,
  GAME_STATE          = 0x4,

} PacketType;

// Packet Struct, might separate this into client and server packet
typedef struct{

  u8 type; 
  u8 seq;
  u8 client;
  union {
    ConnectionRequest connection_request;
    ConnectionResponse connection_response;
    PlayerInput player_input;
    GameState game_state; // bottlenecking the size of the union
  };

}Packet;

#endif // !TRANSPORT_H


