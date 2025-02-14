#include<stdio.h>
#include "map_utils.h"
#include"raylib.h"
#include "world.h"
#include"map_editor.h"
#include"player.h"


void init_test_columns(void);

void init_player(void)
{

    /*Player* player = NULL;*/
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };    
    camera.fovy = (float)PLAYER_FOV;             
    camera.projection = CAMERA_PERSPECTIVE;     
    printf(" player and camera init\n");

    // set camera to the player's 
    /*player->camera = camera;*/
    /**/
    /*printf("Player camera set\n");*/

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Player");
    load_map_layout("map_layout.txt");
    generate_world_walls();
    printf("World walls generated\n");


    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);            
    while (!WindowShouldClose())  
    {

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);


                init_test_columns();
                /*printf("[%f, %f, %f]\n", camera.position.x, camera.position.y, camera.position.z);*/

            EndMode3D();

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

}

void init_test_columns(void)
{

    /*DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground*/
    /*DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall*/
    /*DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall*/
    /*DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);*/
    /*DrawCubeV((Vector3){1.0f, 1.0f, 0.0f}, (Vector3){5.0f, 5.0f, 1.0f}, BLUE);*/
    draw_world_walls();
}

