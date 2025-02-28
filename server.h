#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include "types.h"

#define MAX_CLIENTS 5
#define TICK_RATE 60

typedef struct {

  struct sockaddr_in client_addr;
  u8 client_id;

} ClientInfo;

typedef enum{

  GAME_LOADING = 0x1,
  GAME_ONGOING = 0x2,
  GAME_ENDING  = 0x3,

} GameStatus;

int init_server(char* host);

#endif // !SERVER_H

