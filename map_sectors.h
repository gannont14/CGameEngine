// test
#ifndef MAP_SECTORS_H
#define MAP_SECTORS_H

#include "constants.h"
#include "map_nodes.h"
#include "raylib.h"

typedef struct {

    map_node* map_node_1;
    map_node* map_node_2;
    Color color;
    Vector2 vec;

} map_sector;

extern map_sector map_sectors[MAX_MAP_SECTORS];
extern int num_map_sectors;

int generate_map_sector(map_node* node1, map_node* node2);
void generate_default_map_sectors(void);
void draw_map_sectors(void);
void clear_map_sectors(void);
void split_map_sector(map_sector* ms);

#endif
