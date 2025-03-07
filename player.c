#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "constants.h"
#include "map_utils.h"
#include "raylib.h"
#include "raymath.h"
/*#include "transport.h"*/
#include "server.h"
#include "world.h"
#include "player.h"

#include <time.h>
#include <unistd.h>


void init_test_columns(void);
bool check_collision(Vector3 position, Vector3 movement);
void update_player_to_server_position(void);

int num_players = 0;
Player* players;
Camera3D camera = { 0 };
int player_id;
float interpolation_factor = 0.2f;

Vector3 default_camera_target = (Vector3) { 1.0f, 0.0f, 0.0f};
Vector3 default_camera_up = (Vector3){ 0.0f, 1.0f, 0.0f };    
Vector3 default_player_pos = (Vector3){ 0.60f, 5.0f, 0.60f };    

void check_player_input(Vector3 position, Vector3* movement)
{
  double delta_time = GetFrameTime();
  /*double SECONDS_PER_TICK = 1.0f / (float)TICK_RATE;*/
  /*double client_move_speed = PLAYER_MOVE_SPEED * SECONDS_PER_TICK;*/
  /*client_move_speed *= delta_time * TICK_RATE;*/
  double client_move_speed = PLAYER_MOVE_SPEED * delta_time * 3.0f;
  /*double client_move_speed = CLIENT_MOVE_SPEED;*/


  // reset movement to none
  *movement = (Vector3){0.0f, 0.0f, 0.0f};
  Vector3 new_movement = (Vector3){0.0f, 0.0f, 0.0f};
  // list player movement keybinds
  if(IsKeyDown(KEY_W)) { new_movement.x += client_move_speed; }
  if(IsKeyDown(KEY_S)) { new_movement.x -= client_move_speed; }
  if(IsKeyDown(KEY_A)) { new_movement.y -= client_move_speed; }
  if(IsKeyDown(KEY_D)) { new_movement.y += client_move_speed; }

  // up and down
  if(IsKeyDown(KEY_SPACE)) { new_movement.z += client_move_speed; }
  if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_C)) { new_movement.z -= client_move_speed; }
  /*printf("Attempting to move player to [%f, %f, %f]\n", new_movement.x, new_movement.y, new_movement.z);*/

  printf("CLIENT MOVEMENT: dt=%.6f, speed=%.6f, movement=[%.6f, %.6f, %.6f]\n",
       delta_time, client_move_speed, new_movement.x, new_movement.y, new_movement.z);
  // check to make sure movement won't collide with any walls
  /*if(!check_collision(position, new_movement))*/
  /*{*/
  /*}*/
  *movement = new_movement;
}

void update_player_to_server_position(void)
{
  if(players == NULL) return;
  static int num_corrections = 0;

   pthread_mutex_lock(&game_state_mutex);
  
  int player_index = player_id - 1;
  
  // Calculate distance to server position
  Vector3 server_pos = players[player_index].pos;
  Vector3 current_pos = camera.position;
  Vector3 distance = Vector3Subtract(server_pos, current_pos);
  
  // Only correct if significant difference exists (prevent jitter)
  float dist_magnitude = Vector3Length(distance);
  if(dist_magnitude > 0.5f) {
    
    // Calculate interpolation step
    Vector3 step = Vector3Scale(distance, interpolation_factor);
    
    // Apply correction to camera
    camera.position = Vector3Add(camera.position, step);
    
    // Update player position
    players[player_index].pos = camera.position;
  }
  
  pthread_mutex_unlock(&game_state_mutex);

}


void draw_debug_info(void)
{

  if(players == NULL) return;
  pthread_mutex_lock(&game_state_mutex);
  /*printf("Drawing %d players\n", num_players);*/
  int xPos = 10;
  int yPos = 10;
  int fontSize = 20;
  for(int i = 0; i < num_players; i++)
  {
      DrawText(TextFormat("Player %d: [%.2f, %.2f, %.2f]", 
                         players[i].client_id,
                         players[i].pos.x, 
                         players[i].pos.y, 
                         players[i].pos.z), 
               xPos + 10, yPos, fontSize - 4, RED);

      yPos += fontSize;
  }
  pthread_mutex_unlock(&game_state_mutex);
}

void draw_players(void)
{
  pthread_mutex_lock(&game_state_mutex);
  /*printf("Drawing %d players\n", num_players);*/
  for(int i = 0; i < num_players; i++)
  {
    if(i != player_id - 1)
    {
      /*printf("Drawing player %d at [%f, %f, %f]\n",*/
      /*       players[i].client_id,*/
      /*       players[i].pos.x,*/
      /*       players[i].pos.y,*/
      /*       players[i].pos.z*/
      /*       );*/

      printf("Client: Player %d's camera target: [%f, %f, %f]\n",
             players[i].client_id,
             players[i].camera.target.x,
             players[i].camera.target.y,
             players[i].camera.target.z
             );
      DrawSphere(players[i].pos, 4.0f, RED);
      DrawRay((Ray){players[i].pos, players[i].camera.target}, BLUE);
    }
  }
  pthread_mutex_unlock(&game_state_mutex);
}

bool check_collision(Vector3 position, Vector3 movement)
{
  Vector3 new_pos = Vector3Add(position, movement);
  BoundingBox player_box = (BoundingBox){
    (Vector3){
      new_pos.x - PLAYER_RADIUS,
      new_pos.y - PLAYER_RADIUS,
      new_pos.z - PLAYER_RADIUS
    },
    (Vector3){
      new_pos.x + PLAYER_RADIUS,
      new_pos.y + PLAYER_RADIUS,
      new_pos.z + PLAYER_RADIUS
    }
  };
  /*printf("Player box from \n[%f, %f, %f] - [%f, %f, %f]\n",*/
         /*player_box.min.x, player_box.min.y, player_box.min.z, player_box.max.x, player_box.max.y, player_box.max.z);*/
  DrawBoundingBox(player_box, BLUE);

  for(int i = 0; i < num_world_walls; i++)
  {
    if(CheckCollisionBoxes(player_box, world_walls[i].bounding_box))
    {
      printf("colliding with wall %d\n", i);
      return true;
    }
  }

  return false;
}

void init_player(int id)
{
    player_id = id;
    if (players == NULL) 
    {
        printf("ERROR: players array is NULL in init_player\n");
        exit(EXIT_FAILURE);
    }


    camera.position = default_player_pos;
    camera.target = default_camera_target;
    camera.up = default_camera_up;
    camera.fovy = (float)PLAYER_FOV;             
    camera.projection = CAMERA_PERSPECTIVE;     

    printf("Before, player created with id: %d\n", id);
    pthread_mutex_lock(&game_state_mutex);
    players[id - 1].pos = default_player_pos;
    printf("strat postii set\n");
    players[id - 1].camera = camera;
    pthread_mutex_unlock(&game_state_mutex);
    printf("After\n");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Player");
    load_map_layout("map_layout.txt");
    generate_world_walls();
    /*printf("World walls generated\n");*/

    Vector3 movement;

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(CLIENT_TARGET_FPS);            
    while (!WindowShouldClose())  
    {

    check_player_input(camera.position, &movement);

    UpdateCameraPro(&camera,
                    movement,
                    (Vector3){
                    GetMouseDelta().x * PLAYER_MOUSE_SENSITIVITY,
                    GetMouseDelta().y * PLAYER_MOUSE_SENSITIVITY,
                    0.0f
                    },
                    0.0f);

    /*update_player_to_server_position();*/

    BeginDrawing();            // Begin Drawing -------------------------------------------

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);          // Mode 3D -------------------------------

    draw_world_walls();
    draw_world_floor();
    draw_players();

    EndMode3D();                  // Mode 3D -------------------------------
    draw_debug_info();

    EndDrawing();              // Begin Drawing -------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

}
