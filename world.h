#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "map_sectors.h"

#define WORLD_WALL_HEIGHT 10
#define WORLD_WALL_LENGTH 3
#define WORLD_WALL_COLOR BLUE
#define WORLD_WIDTH 1200
#define WORLD_HEIGHT 800

typedef struct {

  Model model;
  BoundingBox bounding_box;
  Vector3 pos;
  Vector3 r;
  float r_amt;
  float height;
  float width;
  float length;
  Color color;

} World_Wall;

extern World_Wall world_walls[MAX_MAP_SECTORS];
extern int num_world_walls;

void draw_world_walls(void);
void draw_world_floor(void);
void generate_world_walls(void);
#endif // !WORLD_H
