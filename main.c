#include<stdio.h>
#include <sys/types.h>
#include<sys/types.h>
#include<unistd.h>
#include"map_editor.h"
#include"raylib.h"


void init_screen(void);

int main(void)
{

  init_map_editor();
  /*pid_t p;*/
  /**/
  /*p = fork();*/
  /**/
  /*if(p<0) { perror("Fork error"); } // error in process*/
  /*else if( p == 0 ) { init_map_editor(); } // child process*/
  /*else { init_screen(); } // parent process*/
  return 0;

}


void init_screen(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Test title");

    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("TEST TEST TEST TEST", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
}




