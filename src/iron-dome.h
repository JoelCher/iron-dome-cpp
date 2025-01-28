#ifndef _IRON_DOME_H
#define _IRON_DOME_H
#include "main.h"
#include "rocket/rocket.h"
int iron_dome_program(void);
#define GRID_SIZE 100
#define MAX_ENEMY_ROCKET_NUM 100
#define DEFENDER_ROCKET_NUM 200
#define ROCKET_PER_CAPSULE 50
#define HIT_SUCCESS 30
#define HIT_MISS 20
#define FLYING 10
#define WORLD_WIDTH 700
#define WORLD_LENGTH 700

typedef struct {
    Vector3 pos;
    int remaining_rockets;
} Capsule;
typedef struct {
    Vector3 pos;
    Vector3 size;
    Color color;
    bool is_placed;
} Building;

void update_positions();
void check_rockets_collision();
void draw_text_box(char *text, int x, int y, int width, int height, Color color,
                   void (*on_click)());
void reset_game();
void handle_camera(Camera *camera);
void add_enemy_rocket();

void add_building();
#endif
