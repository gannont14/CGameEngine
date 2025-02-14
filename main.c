#include<stdio.h>
#include <sys/types.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include"map_editor.h"
#include"raylib.h"
#include"player.h"


void init_screen(void);

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    perror("usage:\n main: map, player");
    return 1;
  }

  
  if(strcmp(argv[1], "map") == 0)
  {
    init_map_editor();
  }
  if(strcmp(argv[1], "player") == 0)
  {
    init_player();
  }

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




