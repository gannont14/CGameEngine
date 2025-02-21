#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include "types.h"

typedef struct {

  struct sockaddr_in client_addr;
  u16 client_id;

} ClientInfo;

enum GameStatus{

  GAME_LOADING = 0x1,
  GAME_ONGOING = 0x2,
  GAME_ENDING  = 0x3,

};

int init_server(char* host);

#endif // !SERVER_H

