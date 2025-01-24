#include<stdio.h>
#include"map_nodes.h"
#include"constants.h"
#include "raylib.h"



map_node map_nodes[MAX_MAP_NODES];
int num_map_nodes = 0;

// hard coded for now
void generate_default_map_nodes(void)
{
  map_node node1 = (map_node){(Vector2){100.0f, 100.0f}, MAP_NODE_RADIUS, 0, WHITE};
  map_node node2 = (map_node){(Vector2){200.0f, 100.0f}, MAP_NODE_RADIUS, 1, WHITE};
  map_node node3 = (map_node){(Vector2){200.0f, 200.0f}, MAP_NODE_RADIUS, 3, WHITE};
  map_node node4 = (map_node){(Vector2){100.0f, 200.0f}, MAP_NODE_RADIUS, 2, WHITE};

  num_map_nodes = 4;

  map_nodes[0] = node1;
  map_nodes[1] = node2;
  map_nodes[2] = node3;
  map_nodes[3] = node4;
}

int generate_map_node(Vector2 pos)
{
  int ind = num_map_nodes;
  map_nodes[ind] = (map_node){pos, MAP_NODE_RADIUS, ind, WHITE};
  num_map_nodes++;

  return ind;
}

void clear_map_nodes(void)
{
  num_map_nodes = 0;
}

void draw_map_nodes(void)
{
  for(int i = 0; i < num_map_nodes; i++)
  {
    map_node curr = map_nodes[i];
    DrawCircle(curr.pos.x, curr.pos.y, curr.r, curr.color);
  }
}

void draw_map_node_connections(void)
{
  for(int i = 1; i < num_map_nodes + 1; i++)
  {
    map_node prev = map_nodes[i - 1];
    map_node curr = map_nodes[i % num_map_nodes];
    /*DrawLine(curr.x, curr.y, prev.x, prev.y, WHITE);*/
    DrawLineEx(curr.pos, prev.pos, MAP_SECTOR_THICKNESS, WHITE);
  }
}


















