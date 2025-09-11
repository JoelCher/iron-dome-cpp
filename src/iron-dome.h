#pragma once
#include "entity/Entity.h"
#include "gui-element/GuiElement.h"
#include "main.h"
#include "rocket/enemy-rocket.h"
#include "rocket/rocket.h"
#include "tank/Tank.h"
#include <stdlib.h>
#include <vector>
#define GRID_SIZE 100
#define MAX_ENEMY_ROCKET_NUM 100
#define DEFENDER_ROCKET_NUM 200
#define ROCKET_PER_CAPSULE 50
#define HIT_SUCCESS 30
#define HIT_MISS 20
#define FLYING 10
#define WORLD_WIDTH 1400.0f
#define WORLD_LENGTH 700.0f

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

class IronDomeProgram {
  public:
    // Every rocket launch has a cost..
    float moneyWasted = 0;
    Camera camera;
    std::vector<DefenderRocket> defenderRockets;
    std::vector<EnemyRocket> enemyRockets;
    std::vector<GuiElement> guiElements;

    std::vector<Capsule> ironDomes = {
        {{40, 0, -200}, 50}, {{40, 0, 200}, 50}, {{40, 0, 0}, 50}};

    Building newBuilding = {{-100, 50.0 / 2, 30}, {3, 50, 5}, WHITE};
    bool isAddingBuilding = false;
    std::vector<Building> buildings;

    bool isSurrounding = false;
    struct {
        Vector3 start;
        Vector3 end;
    } mouse_surround_pos;
    // Entities selected by the mouse surround or by single click
    std::vector<Tank *> selectedEntities;
    std::vector<Tank> tanks;
    Vector3 selectedEntitiesTarget;
    bool has_selected_target_position = false;

    int init();
    void check_rockets_collision();
    void update_positions();
    void reset_world();
    void add_building();
    void handle_mouse(Camera *camera);
    void handle_camera(Camera *camera);
    void add_enemy_rocket();
    void loop();
    void draw();
    void escapeDefenderRockets();
};
