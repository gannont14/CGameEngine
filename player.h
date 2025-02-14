#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
typedef struct{

  Vector2 pos;
  Camera camera;

} Player;

#define PLAYER_FOV 60
#define PLAYER_CAMERA_MODE CAMERA_FIRST_PERSON

void init_player(void);

#endif // !PLAYER_H
