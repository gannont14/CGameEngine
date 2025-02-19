#include <stdio.h>
#include "world.h"
#include "constants.h"
#include "map_sectors.h"
#include"raylib.h"
#include "raymath.h"
#include"world.h"

World_Wall world_walls[MAX_MAP_SECTORS];
int num_world_walls = 0;

World_Wall create_world_wall(map_sector ms);
void draw_world_wall(World_Wall ww);
void draw_world_walls(void);
Model generate_wall_model(float height, float width, float length);
float calculate_y_rotation(map_sector ms);
Vector2 map_to_world_pos(Vector2 vec);

void draw_world_floor(void)
{
  DrawPlane((Vector3){SCREEN_WIDTH / 20.0f, 0.0f, SCREEN_HEIGHT / 20.0f}, (Vector2){SCREEN_WIDTH / 10.0f, SCREEN_HEIGHT / 10.0f}, GRAY);
}

void draw_world_walls(void)
{
  for(int i = 0; i < num_world_walls; i++)
  {
    draw_world_wall(world_walls[i]);
    DrawBoundingBox(world_walls[i].bounding_box, RED);
  }
}

void draw_world_wall(World_Wall ww)
{
  DrawModelEx(ww.model, ww.pos, ww.r, ww.r_amt, (Vector3){ 1.0f, 1.0f, 1.0f }, ww.color);
}

void generate_world_walls(void)
{
  for(int i = 0; i < num_map_sectors; i++)
  {
    world_walls[i] = create_world_wall(map_sectors[i]);
    num_world_walls++;
  }

}

World_Wall create_world_wall(map_sector ms)
{


  Vector2 mn1 = map_to_world_pos(ms.map_node_1->pos);
  Vector2 mn2 = map_to_world_pos(ms.map_node_2->pos);


  /*printf("Creating world wall\n");*/
  // generate width of the wall, which should just be the length of the vector in the ms
  float wall_width = Vector2Distance(mn1, mn2);
  float wall_height = WORLD_WALL_HEIGHT * 1.0f;
  float wall_length = WORLD_WALL_LENGTH * 0.5f;

  // generate the wall's model for generation
  Model wall_model = generate_wall_model(wall_height, wall_width, wall_length);
  // generate the walls bouding box

  // generating the wall's texture
  Texture2D wall_texture;
  char* texture_file_path;
  texture_file_path = "textures/brick_wall_texture.png";
  wall_texture = LoadTexture(texture_file_path);
  // setting wall texture
  wall_model.materials->maps[0].texture = wall_texture;
  /*wall_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].value = wall_width / 2.0f;  // Scale X*/
  
  // generate a midpoint for the vector
  Vector3 mid_vec = (Vector3){
    (mn1.x + mn2.x ) / (2.0f),
    wall_height / 2.0f,
    (mn1.y + mn2.y ) / (2.0f),
  };

  // generate the roation between the two points
  float wall_rotation = -1.0f * calculate_y_rotation(ms);
  
  /*printf("Generating wall at: [%f, %f, %f] with a rotation of %f, distance of %f\n", mid_vec.x, mid_vec.y, mid_vec.z, wall_rotation, wall_width);*/
  BoundingBox bounding_box = GetModelBoundingBox(wall_model);
  World_Wall wall = (World_Wall){
    wall_model,
    bounding_box,
    mid_vec,
    (Vector3){0.0f, 1.0f, 0.0f}, // Hard coded to only be able to rotate on y axis, could change?
    wall_rotation,
    wall_height,
    wall_width,
    wall_length,
    WHITE
  };

  return wall;
}


Vector2 map_to_world_pos(Vector2 vec)
{
  return (Vector2){vec.x / 10.0f, vec.y / 10.0f};
}

float calculate_y_rotation(map_sector ms)
{
    // Get the direction vector between points
    float dx = ms.map_node_2->pos.x - ms.map_node_1->pos.x;
    float dz = ms.map_node_2->pos.y - ms.map_node_1->pos.y;
    
    float angle = atan2f(dz, dx);
    float degrees = angle * RAD2DEG;

    return degrees;
}


Model generate_wall_model(float height, float width, float length)
{
  Mesh wall_mesh = GenMeshCube(width, height, length);
  return LoadModelFromMesh(wall_mesh);
}



