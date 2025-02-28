#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
typedef struct{

  Vector3 pos;
  Camera3D camera;
  int client_id;

} Player;

#define PLAYER_FOV 60
#define PLAYER_CAMERA_MODE CAMERA_FIRST_PERSON
#define PLAYER_MOVE_SPEED 5.0f
#define PLAYER_MOUSE_SENSITIVITY 0.25f
#define PLAYER_RADIUS 1.5f

#define CLIENT_MOVE_SPEED 0.5f     
#define SERVER_MOVE_SPEED 0.008333f  

#define PLAYER_UP       0b100000
#define PLAYER_DOWN     0b010000
#define PLAYER_FORWARD  0b001000
#define PLAYER_BACKWARD 0b000100
#define PLAYER_LEFT     0b000010
#define PLAYER_RIGHT    0b000001

#define CLIENT_TARGET_FPS 60

void init_player(int id);

extern int num_players;
extern Player* players;
extern Camera3D camera;

#endif // !PLAYER_H
