#include<stdio.h>
#include "map_sectors.h"
#include "constants.h"
#include "map_nodes.h"
#include "raylib.h"
#include "raymath.h"

map_sector map_sectors[MAX_MAP_SECTORS];
int num_map_sectors = 0;


void generate_default_map_sectors(void)
{

  map_node node1 = (map_node){(Vector2){100.0f, 100.0f}, MAP_NODE_RADIUS, 0, RAYWHITE};
  map_node node2 = (map_node){(Vector2){200.0f, 100.0f}, MAP_NODE_RADIUS, 1, RAYWHITE};

  generate_map_sector(&node1, &node2);
  printf("Generated default map sectors");

}


// Generates a map sector, calculating it's vector as well
int generate_map_sector(map_node* node1, map_node* node2)
{
  
  // subtract node1 from node2 to get vector to add
  Vector2 v = Vector2Subtract(node2->pos, node1->pos);
  int ind = num_map_sectors;

  map_sectors[ind] = (map_sector){node1, node2, MAP_SECTOR_COLOR, v};
  num_map_sectors++;

  return ind;

}


// Draw the map sectors 
void draw_map_sectors(void)
{

  for(int i = 0; i < num_map_sectors; i++)
  {
    map_sector* curr = &map_sectors[i];
    DrawLineEx(curr->map_node_1->pos, curr->map_node_2->pos ,MAP_SECTOR_THICKNESS, MAP_SECTOR_COLOR);
  }

}

void clear_map_sectors(void)
{
  num_map_sectors = 0;
}

void split_map_sector(map_sector* ms)
{

  // generate new map node in the center of the vector 
  // find average of x and y components
  float avg_x = (ms->map_node_1->pos.x + ms->map_node_2->pos.x) / 2.0f;
  float avg_y = (ms->map_node_1->pos.y + ms->map_node_2->pos.y) / 2.0f;

  printf("Found midpoint at %f, %f\n", avg_x, avg_y);

  Vector2 mid_vec = (Vector2){avg_x, avg_y};

  // keep track of old mn2
  map_node* og_map_node_2 = ms->map_node_2;

  // generate a new node at this position, and set current ms from mn1 -> mid , and generate new 
  int new_map_node_ind = generate_map_node(mid_vec);
  map_node* new_map_node = &map_nodes[new_map_node_ind];

  ms->map_node_2 = new_map_node;

  /*int new_map_sector_ind = generate_map_sector(new_map_node, og_map_node_2);*/
  generate_map_sector(new_map_node, og_map_node_2);

}











