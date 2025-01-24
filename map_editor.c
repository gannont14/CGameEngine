#include<stdio.h>
#include "map_nodes.h"
#include "map_sectors.h"
#include "map_utils.h"
#include"raylib.h"
#include"raymath.h"
/*#include"map_nodes.c"*/
#include "constants.h"
/*#include "map_sectors.c"*/
#include "map_editor.h"



void init_map_editor(void){



  // generate the default map nodes
  /*generate_default_map_nodes();*/

  /*generate_default_map_sectors();*/


  load_map_layout(map_save_file_location);



  // -1 for default, nothign being done
  int held_node_index = -1;

  // Bool flags, could be changed for bool
  int nodes_connecting = 0;
  int prev_map_node_index = -1;
  int can_hover_sector = 1;
  int hovered_map_sector_index = -1;
  /*map_node curr_node;*/

  InitWindow(SCREEN_WIDTH ,SCREEN_HEIGHT , "Map Editor");
  SetTargetFPS(60);               

  while (!WindowShouldClose())    
  {
    can_hover_sector = 1;
    hovered_map_sector_index = -1;

    // Checking the mouse for dragging

    // Check the nodes for hovering
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < num_map_nodes; i++)
    {

      map_node curr = map_nodes[i];
      if(CheckCollisionCircles(mouse, MAP_NODE_RADIUS, curr.pos, curr.r))
      {
        // double the size of the circle 
        DrawCircleV(curr.pos, MAP_NODE_RADIUS * 2, HOVERED_NODE_COLOR );
        can_hover_sector = 0;
        /*map_nodes[i].color = BLUE;*/
        /*map_nodes[i].r = curr.r;*/
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
          held_node_index = i;
          /*printf("Button pressed at index: %d\n", i);*/
        }
      }

    }

    if(can_hover_sector == 1)
    {
      
      // Check the sectors for hovering
      for(int i = 0; i < num_map_sectors; i++)
      {

        map_sector curr = map_sectors[i];
        if(CheckCollisionPointLine(mouse, curr.map_node_2->pos, curr.map_node_1->pos, 11))
        {
          // colliding with sector, draw the hover color
          /*printf("Collision with sector : %d\n", i);*/
          DrawLineEx(curr.map_node_2->pos , curr.map_node_1->pos, MAP_SECTOR_THICKNESS * 2, RED);
          hovered_map_sector_index = i;
        }

      }

    }


    // if mouse is currently dragging a node
    if(held_node_index != -1)
    {

      map_nodes[held_node_index].pos.x = mouse.x;
      map_nodes[held_node_index].pos.y = mouse.y;
      if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      {

        held_node_index = -1;

      }

    }


    // KEYBINDINGS -------------------
    if(IsKeyPressed(KEY_G))
    {

      int brk = 0;

      /*map_node* prev_map_node = NULL;*/
      if(nodes_connecting == 0)
      {

        // check to see if colliding
        for(int i = 0; i < num_map_nodes; i++)
        {
          map_node curr = map_nodes[i];
          if(CheckCollisionCircles(mouse, MAP_NODE_RADIUS, curr.pos, curr.r))
          {

            prev_map_node_index = i;
            nodes_connecting = 1;
            brk = 1;

          }
        }

        if(brk == 0)
        {

          printf("First node placed, just generating node\n");
          generate_map_node(mouse);
          /*prev_map_node = &map_nodes[num_map_nodes - 1];*/
          prev_map_node_index = num_map_nodes - 1;
          printf("prev_node_index should now be: %d\n", num_map_nodes - 1);
          printf("prev_node_index is now: %d\n", prev_map_node_index);
          nodes_connecting = 1;

        }

      }
      else if(nodes_connecting == 1)
      {

        printf("nodes connecting\n");
        printf("num_nodes : %d\n", num_map_nodes);
        int touching = 0;
        // check first if the mouse is colliding 
        for(int i = 0; i < num_map_nodes; i++)
        {
          printf("%d loop of for loop\n", i);
          map_node curr = map_nodes[i];
          if(CheckCollisionCircles(mouse, MAP_NODE_RADIUS, curr.pos, curr.r))
          {
            printf("mouse colliding with node when G pressed\n");
            touching = 1;
            // the mouse is colliding with an existing node, make a sector that 
            generate_map_sector(&map_nodes[prev_map_node_index], &map_nodes[i]);
            prev_map_node_index = -1;
            nodes_connecting = 0;
          }
        }

        /*printf("Through for loop\n");*/
        printf("Touching: %d\n", touching);

        if(touching == 0)
        {
          printf("Node not touching, generating new node and sector\n");

          // the mouse is not connecting
          // generate a new map node, and a new sector connecting to the prev
          generate_map_node(mouse);
          printf("Making sector from node indeces %d - %d\n", prev_map_node_index, num_map_nodes - 1);
          generate_map_sector(&map_nodes[prev_map_node_index], &map_nodes[num_map_nodes - 1]);
          prev_map_node_index++;
          
        }

        printf("Made it to end\n");

      }
    }

    if(IsKeyPressed(KEY_C))
    {
      // Clear all the map nodes
      clear_map_nodes();
      clear_map_sectors();
    }

    if(IsKeyPressed(KEY_B))
    {
      // Clear all the map nodes
      if(hovered_map_sector_index != -1)
      {

        printf("Splitting map sector %d\n", hovered_map_sector_index);
        map_sector* curr_sector = &map_sectors[hovered_map_sector_index];
        split_map_sector(curr_sector);

      }
    }
    if(IsKeyPressed(KEY_S))
    {
      
      // save the current state of the map
      save_map_layout(map_save_file_location);

    }


    // end KEYBINDINGS --------------


    BeginDrawing(); 


      if(DEBUG_CURSOR)
      {

      DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), 
                      Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, RAYWHITE);

      }

      ClearBackground(BLACK);

      draw_map_nodes();
      /*draw_map_node_connections();*/
      draw_map_sectors();

    EndDrawing();
  }

  CloseWindow();
}




