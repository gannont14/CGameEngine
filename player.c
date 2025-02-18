#include<stdio.h>
#include<stdlib.h>
#include "constants.h"
#include "map_utils.h"
#include"raylib.h"
#include "world.h"
#include"map_editor.h"
#include"player.h"


void init_test_columns(void);

void init_player(void)
{
    Vector3 player_start_pos = (Vector3){SCREEN_WIDTH / 20.0f, 10.0f, SCREEN_HEIGHT / 20.0f};

    Camera3D camera = { 0 };
    camera.position = player_start_pos;
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };    
    camera.fovy = (float)PLAYER_FOV;             
    camera.projection = CAMERA_PERSPECTIVE;     

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Player");
    load_map_layout("map_layout.txt");
    generate_world_walls();
    printf("World walls generated\n");


    DisableCursor();                    // Limit cursor to relative movement inside the window


    Vector3 movement;

    SetTargetFPS(60);            
    while (!WindowShouldClose())  
    {
      movement.x = 0.0f;
      movement.y = 0.0f;
      movement.z = 0.0f;
      // KEYBINDS - custom movement keybinds

        if(IsKeyDown(KEY_W))
        {
          movement.x += PLAYER_MOVE_SPEED;
        }
        if(IsKeyDown(KEY_S))
        {
          movement.x -= PLAYER_MOVE_SPEED;
        }
        if(IsKeyDown(KEY_A))
        {
          movement.y -= PLAYER_MOVE_SPEED;
        }
        if(IsKeyDown(KEY_D))
        {
          movement.y += PLAYER_MOVE_SPEED;
        }
    
        // up and down
        if(IsKeyDown(KEY_SPACE))
        {
          movement.z += PLAYER_MOVE_SPEED;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_C))
        {
          movement.z -= PLAYER_MOVE_SPEED;
        }

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

            EndMode3D();

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

}
