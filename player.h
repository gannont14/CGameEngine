#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
typedef struct{

  Vector3 pos;
  Camera camera;

} Player;

#define PLAYER_FOV 60
#define PLAYER_CAMERA_MODE CAMERA_FIRST_PERSON
#define PLAYER_MOVE_SPEED 0.5f
#define PLAYER_MOUSE_SENSITIVITY 0.25f
#define PLAYER_RADIUS 1.5f

void init_player(void);

extern int num_players;
extern Player* players;

#endif // !PLAYER_H
