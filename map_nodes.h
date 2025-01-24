#ifndef MAP_NODES_H
#define MAP_NODES_H

#include "constants.h"
#include "raylib.h"

typedef struct {

    Vector2 pos;
    float r;
    int i;
    Color color;

} map_node;

extern map_node map_nodes[MAX_MAP_NODES];
extern int num_map_nodes;

void draw_map_nodes(void);
void generate_default_map_nodes(void);
void draw_map_node_connections(void);
int generate_map_node(Vector2 pos);
void clear_map_nodes(void);

#endif
