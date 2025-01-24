#include "constants.h"
#include"map_editor.h"
#include "map_nodes.h"
#include "map_sectors.h"
#include "raymath.h"
#include "map_utils.h"

#include <stdio.h>

/*
  *
  *      FILE LAYOUT
  *      ----------------------------
  *      number of nodes
  *    coords (x,y) of nodes in order...
  *      node 0
  *      node 1
  *      ...
  *      number of sectors
  *    connecting nodes in order
  *      0 1
  *      1 2
  *
  */


char* map_save_file_location = "map_layout.txt";


void save_map_layout(char* filename)
{
  FILE* f = NULL;
  f = fopen(filename, "w");

  if(!f)
  {

    printf("Error opening file: %s\n", filename);
    return;

  }

  // print the number of nodes 
  fprintf(f, "%d\n", num_map_nodes);

  // loop through and print the coords of the nodes now
  for(int i = 0; i < num_map_nodes; i++)
  {

    map_node curr = map_nodes[i];
    fprintf(f, "%f %f\n", curr.pos.x, curr.pos.y);

  }

  // print the number of sectors
  fprintf(f, "%d\n", num_map_sectors);

  // loop through and print the connecting node indeces
  for(int i = 0; i < num_map_sectors; i++)
  {

    map_sector curr = map_sectors[i];
    fprintf(f, "%d %d\n", curr.map_node_1->i, curr.map_node_2->i);

  }

  printf("saved map layout to file %s\n", filename);
  fclose(f);

}


void load_map_layout(char* filename)
{

  FILE* f = NULL;
  f = fopen(filename, "r");
  if(!f)
  {
    printf("Error opening file on load\n");
    return;
  }

  // read the number of nodes
  fscanf(f, "%d", &num_map_nodes);
  printf("Read %d map nodes\n", num_map_nodes);

  // read through the coords and create map node
  for(int i = 0; i < num_map_nodes; i++)
  {
    float x, y;
    fscanf(f, "%f %f", &x, &y);
    map_nodes[i] = (map_node){
      (Vector2){x, y},
      MAP_NODE_RADIUS,
      i,
      MAP_NODE_COLOR
    };
  }

  // read now the number of map sectors 
  fscanf(f, "%d", &num_map_sectors);
  // read through the nodes and add them to the 
  for(int i = 0; i < num_map_sectors; i++)
  {

    int ind1, ind2;
    fscanf(f, "%d %d",&ind1, &ind2);
    map_sectors[i] = (map_sector){

      &map_nodes[ind1],
      &map_nodes[ind2],
      MAP_SECTOR_COLOR,
      Vector2Subtract(map_nodes[ind2].pos, map_nodes[ind1].pos)

    };

  }

  fclose(f);

}
