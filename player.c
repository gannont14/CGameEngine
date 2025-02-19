#include <stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include "constants.h"
#include "map_utils.h"
#include"raylib.h"
#include "raymath.h"
#include "world.h"
#include"player.h"


void init_test_columns(void);
bool check_collision(Vector3 position, Vector3 movement);


void check_player_input(Vector3 position, Vector3* movement)
{
        // reset movement to none
        *movement = (Vector3){0.0f, 0.0f, 0.0f};
        Vector3 new_movement = (Vector3){0.0f, 0.0f, 0.0f};
        // list player movement keybinds
        if(IsKeyDown(KEY_W))
        { new_movement.x += PLAYER_MOVE_SPEED; }
        if(IsKeyDown(KEY_S))
        { new_movement.x -= PLAYER_MOVE_SPEED; }
        if(IsKeyDown(KEY_A))
        { new_movement.y -= PLAYER_MOVE_SPEED; }
        if(IsKeyDown(KEY_D))
        { new_movement.y += PLAYER_MOVE_SPEED; }
    
        // up and down
        if(IsKeyDown(KEY_SPACE))
        { new_movement.z += PLAYER_MOVE_SPEED; }
        if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_C))
        { new_movement.z -= PLAYER_MOVE_SPEED; }
        /*printf("Attempting to move player to [%f, %f, %f]\n", new_movement.x, new_movement.y, new_movement.z);*/

      // check to make sure movement won't collide with any walls
        if(!check_collision(position, new_movement))
        {
          *movement = new_movement;
        }
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

void init_player(void)
{
    Vector3 player_start_pos = (Vector3){
      SCREEN_WIDTH / 20.0f, 10.0f, 
      SCREEN_HEIGHT / 20.0f
    };

    Camera3D camera = { 0 };
    camera.position = player_start_pos;
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };    
    camera.fovy = (float)PLAYER_FOV;             
    camera.projection = CAMERA_PERSPECTIVE;     

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Player");
    load_map_layout("map_layout.txt");
    generate_world_walls();
    /*printf("World walls generated\n");*/

    Vector3 movement;

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);            
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


        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

              draw_world_walls();
              draw_world_floor();

            EndMode3D();

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

}
