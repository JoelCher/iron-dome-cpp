#include "./iron-dome.h"
#include "../include/json.hpp"
#include "rocket/rocket.h"
#include "gui-element/GuiElement.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
using json = nlohmann::json;
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
using namespace std;

vector<DefenderRocket> defenderRockets;
vector<EnemyRocket> enemyRockets;

vector<Capsule> ironDomes = {
    {{40, 0, -200}, 50}, {{40, 0, 200}, 50}, {{40, 0, 0}, 50}};

Building newBuilding = {{-100, 50.0 / 2, 30}, {3, 50, 5}, WHITE};
bool isAddingBuilding = false;
vector<Building> buildings;

bool isSurrounding = false;
struct
{
    Vector3 start;
    Vector3 end;
} mouse_surround_pos;
// Entities selected by the mouse surround or by single click
vector<Tank *> selectedEntities;
vector<Tank> tanks;
Vector3 selectedEntitiesTarget;
bool has_selected_target_position = false;

int iron_dome_program(void)
{
    GuiElement resetButton({SCREEN_WIDTH - 450, SCREEN_HEIGHT - 100, 120, 30});
    GuiElement addBuildingButton({SCREEN_WIDTH - 650, SCREEN_HEIGHT - 100, 200, 30});
    GuiElement shootRocketButton({SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100, 300, 30});
    // Loading json data about buildings
    Model model = LoadModel("../resources/untitled.obj");
    std::ifstream loadFile("buildings.json");
    if (loadFile.is_open())
    {
        json loadedData;
        loadFile >> loadedData; // Load JSON from file
        loadFile.close();
        for (int i = 0; i < loadedData.size(); i++)
        {
            Building building;
            int x = loadedData[i]["position"]["x"];
            int y = loadedData[i]["position"]["y"];
            int z = loadedData[i]["position"]["z"];
            int size_x = loadedData[i]["size"]["x"];
            int size_y = loadedData[i]["size"]["y"];
            int size_z = loadedData[i]["size"]["z"];
            building.pos.x = x;
            building.pos.y = y;
            building.pos.z = z;
            building.color = WHITE;
            building.size.x = size_x;
            building.size.y = size_y;
            building.size.z = size_z;
            buildings.push_back(building);
        }
    }
    else
    {
        cerr << "Failed to open the file for reading." << std::endl;
    }
    // Initializing the defender rockets
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * ironDomes.size();
         i++)
    {
        // Setting at -1 means they have not been launched yet
        DefenderRocket newRocket;
        newRocket.rocketTarget = -1;
        // Each iron dome capsule receives 50 rockets
        int ironDomeId = i / ROCKET_PER_CAPSULE;
        newRocket.ironDomeId = ironDomeId;
        newRocket.pos.x = ironDomes[ironDomeId].pos.x;
        newRocket.pos.y = 1.0 / 2;
        newRocket.pos.z = ironDomes[ironDomeId].pos.z;
        newRocket.length = 2;
        defenderRockets.push_back(newRocket);
    }

    // Define the camera to look into our 3d world
    // ChangeDirectory(GetApplicationDirectory());
    Camera camera = {0};
    camera.position =
        (Vector3){0.0f, 300.0f, WORLD_LENGTH / 2.0 + 40}; // Camera position
    camera.target = (Vector3){
        0.0f, 0.0f, WORLD_LENGTH / 2.0 - 200}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f,
                          0.0f};            // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    Tank new_tank;
    new_tank.pos = {-200, 10, 100};
    new_tank.bounding_box = {.min = {new_tank.pos.x, 0, new_tank.pos.z}, {new_tank.pos.x, new_tank.pos.y + 10, new_tank.pos.z}};
    tanks.push_back(new_tank);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        handle_camera(&camera);
        handle_mouse(camera);

        // Assigning targets to defender rockets
        // We need to check if there are any enemy rockets
        if (enemyRockets.size())
        {
            for (int i = 0; i < enemyRockets.size(); i++)
            {
                // Check if there is already a defender rocket assigned to this
                // rocket
                bool is_already_assigned = false;
                for (int j = 0; j < DEFENDER_ROCKET_NUM; j++)
                {
                    if (defenderRockets[j].rocketTarget == i)
                        is_already_assigned = true;
                }
                if (!is_already_assigned)
                {
                    // Then assign some defender rocket to this enemy rocket
                    // Check which capsule is closest to that rocket
                    EnemyRocket curr_enemy_rocket = enemyRockets[i];
                    int closest_capsule = 0;
                    double closest_distance = -1;
                    for (int z = 0; z < ironDomes.size(); z++)
                    {
                        Capsule curr_capsule = ironDomes[z];
                        double x_diff =
                            curr_enemy_rocket.pos.x - curr_capsule.pos.x;
                        double y_diff =
                            curr_enemy_rocket.pos.y - curr_capsule.pos.y;
                        double z_diff =
                            curr_enemy_rocket.pos.z - curr_capsule.pos.z;
                        double distance =
                            sqrt(x_diff * x_diff + y_diff * y_diff +
                                 z_diff * z_diff);
                        if (distance < closest_distance ||
                            closest_distance == -1)
                        {
                            closest_distance = distance;
                            closest_capsule = z;
                        }
                    }
                    for (int j = 0; j < DEFENDER_ROCKET_NUM; j++)
                    {
                        if (defenderRockets[j].ironDomeId ==
                                closest_capsule &&
                            defenderRockets[j].rocketTarget == -1)
                        {
                            defenderRockets[j].rocketTarget = i;
                            defenderRockets[j].status = FLYING;
                            ironDomes[defenderRockets[j].ironDomeId]
                                .remaining_rockets--;
                            break;
                        }
                    }
                }
            }
        }

        check_rockets_collision();
        update_positions();

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
        // Draw around all selected entities
        for (const Tank *entity : selectedEntities)
        {
            DrawBoundingBox(entity->bounding_box, RED);
        }

        for (const Tank &tank : tanks)
        {
            DrawModel(model, tank.pos, 2, WHITE);
        }

        // Drawing a floor
        DrawPlane((Vector3){0, 0, 0}, (Vector2){WORLD_WIDTH, WORLD_LENGTH},
                  LIGHTGRAY);
        // Drawing the iron dome capsules
        for (int i = 0; i < ironDomes.size(); i++)
        {
            DrawSphere(ironDomes[i].pos, 5, BLUE);
        }
        // Drawing the enemy rocket
        for (int i = 0; i < enemyRockets.size(); i++)
        {
            if (enemyRockets[i].isDestroyed)
                continue;
            // DrawCubeV(enemyRockets[i].pos, enemyRockets[i].size, RED);
            // DrawCubeWiresV(enemyRockets[i].pos, enemyRockets[i].size, RED);
            Vector3 curr_pos = enemyRockets[i].pos;
            Vector3 curr_velocity = enemyRockets[i].velocity;
            // Calc the velocity norm
            double speed = sqrt(curr_velocity.x * curr_velocity.x +
                                curr_velocity.y * curr_velocity.y +
                                curr_velocity.z * curr_velocity.z);
            // The size of the rocket is 2, so I need to find the number to
            // divide bla bla bla(I dont know how to explain, im stupid)
            float d = speed / enemyRockets[i].length;
            DrawCapsule(curr_pos,
                        (Vector3){curr_pos.x - curr_velocity.x / d,
                                  curr_pos.y - curr_velocity.y / d,
                                  curr_pos.z - curr_velocity.z / d},
                        1.2f, 8, 8, VIOLET);
            DrawCapsuleWires(curr_pos,
                             (Vector3){curr_pos.x - curr_velocity.x / d,
                                       curr_pos.y - curr_velocity.y / d,
                                       curr_pos.z - curr_velocity.z / d},
                             1.2f, 8, 8, PURPLE);
        }

        for (int i = 0; i < defenderRockets.size(); i++)
        {
            if (defenderRockets[i].rocketTarget != -1 &&
                defenderRockets[i].status == FLYING)
            {
                Vector3 curr_pos = defenderRockets[i].pos;
                Vector3 curr_velocity = defenderRockets[i].velocity;
                double speed = sqrt(curr_velocity.x * curr_velocity.x +
                                    curr_velocity.y * curr_velocity.y +
                                    curr_velocity.z * curr_velocity.z);
                float d = speed / defenderRockets[i].length;
                DrawCapsule(curr_pos,
                            (Vector3){curr_pos.x - curr_velocity.x / d,
                                      curr_pos.y - curr_velocity.y / d,
                                      curr_pos.z - curr_velocity.z / d},
                            1.2f, 8, 8, BLUE);
                DrawCapsuleWires(curr_pos,
                                 (Vector3){curr_pos.x - curr_velocity.x / d,
                                           curr_pos.y - curr_velocity.y / d,
                                           curr_pos.z - curr_velocity.z / d},
                                 1.2f, 8, 8, BLUE);
            }
        }

        if (has_selected_target_position)
        {
            for (Tank *tank : selectedEntities)
            {
                // Move to that direction
                // I need to shrink the distance vector to the speed of the tank(which is tank.speed)
                Vector3 distance_v = {selectedEntitiesTarget.x - tank->pos.x, 0, selectedEntitiesTarget.z - tank->pos.z};
                float distance_norm = sqrt(distance_v.x * distance_v.x + distance_v.z * distance_v.z);
                if (distance_norm < 1)
                    continue;
                float x_dir = distance_v.x * (tank->speed / distance_norm);
                float z_dir = distance_v.z * (tank->speed / distance_norm);
                tank->pos.z += z_dir / 60.0;
                tank->pos.x += x_dir / 60.0;
            }
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !GuiElement::isClickingGuiElement())
        {
            // Translating the mouse click to the 3d platform
            Vector2 mouse_pos = GetMousePosition();
            Ray ray = GetMouseRay(mouse_pos, camera);
            RayCollision real_mouse_pos = GetRayCollisionBox(
                ray,
                (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                              {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
            if (selectedEntities.size())
            {
                selectedEntitiesTarget = real_mouse_pos.point;
                has_selected_target_position = true;
            }
        }
        if (isAddingBuilding)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !GuiElement::isClickingGuiElement())
            {
                // Place the building
                Building building_to_add;
                building_to_add.pos.x = newBuilding.pos.x;
                building_to_add.pos.y = newBuilding.pos.y;
                building_to_add.pos.z = newBuilding.pos.z;
                building_to_add.size.x = newBuilding.size.x;
                building_to_add.size.y = newBuilding.size.y;
                building_to_add.size.z = newBuilding.size.z;
                building_to_add.color = newBuilding.color;
                buildings.push_back(building_to_add);
                isAddingBuilding = false;
                ShowCursor();

                // Add the building to the json database
                std::ifstream input_file("buildings.json");
                json json_array = json::array();
                input_file >> json_array;
                json j_object;
                j_object["position"] = {{"x", building_to_add.pos.x},
                                        {"y", building_to_add.pos.y},
                                        {"z", building_to_add.pos.z}};
                j_object["size"] = {{"x", building_to_add.size.x},
                                    {"y", building_to_add.size.y},
                                    {"z", building_to_add.size.z}};
                json_array.push_back(j_object);
                std::ofstream file("buildings.json");
                file << json_array.dump(4); // Pretty-print with 4 spaces
                file.close();
            }
            else
            {
                // Draw a building following the mouse
                Vector2 mouse_pos = GetMousePosition();
                Ray pos = GetMouseRay(mouse_pos, camera);
                RayCollision collision = GetRayCollisionBox(
                    pos,
                    (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                                  {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
                newBuilding.pos.x = collision.point.x;
                newBuilding.pos.z = collision.point.z;
                DrawCubeV(newBuilding.pos, newBuilding.size,
                          newBuilding.color);
            }
        }
        // Just drawing a bunch of buildings
        for (int i = 0; i < buildings.size(); i++)
        {
            DrawCubeV(buildings[i].pos, buildings[i].size, buildings[i].color);
        }

        DrawLine3D((Vector3){0, 0, -WORLD_LENGTH / 2},
                   (Vector3){0, 0, WORLD_LENGTH / 2}, RED);

        if (isSurrounding)
        {
            const float size_x =
                (mouse_surround_pos.start.x - mouse_surround_pos.end.x);
            const float size_z =
                (mouse_surround_pos.start.z - mouse_surround_pos.end.z);
            DrawCubeWiresV((Vector3){mouse_surround_pos.start.x - size_x / 2.0,
                                     10,
                                     mouse_surround_pos.start.z - size_z / 2.0},
                           (Vector3){abs(size_x), 0, abs(size_z)}, GREEN);
        }
        EndMode3D();

        GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        if (GuiButton(
                resetButton.pos_rec,
                "Reset"))
        {
            reset_game();
        }
        if (GuiButton(
                shootRocketButton.pos_rec,
                "Shoot enemy rocket"))
        {
            add_enemy_rocket();
        }
        if (GuiButton(
                addBuildingButton.pos_rec,
                "Add building"))
        {
            add_building();
        }
        EndDrawing();
    }

    UnloadModel(model);
    CloseWindow(); // Close window and OpenGL context
    return 0;
}

void check_rockets_collision()
{
    for (int i = 0; i < DEFENDER_ROCKET_NUM; i++)
    {
        // Dont check for destroyed rockets
        if (defenderRockets[i].status != FLYING)
            continue;
        for (int j = 0; j < enemyRockets.size(); j++)
        {
            if (enemyRockets[j].isDestroyed)
                continue;
            double z_diff = enemyRockets[j].pos.z - defenderRockets[i].pos.z;
            double y_diff = enemyRockets[j].pos.y - defenderRockets[i].pos.y;
            double x_diff = enemyRockets[j].pos.x - defenderRockets[i].pos.x;
            // calc square root(size z^2 + y^2 +x^2)
            double diff_norma =
                sqrt(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);
            if (diff_norma < 2)
            {
                // Stop those two rockets
                enemyRockets[j].isDestroyed = true;
                defenderRockets[i].status = HIT_SUCCESS;
            }
        }
    }
}

void update_positions()
{
    int animFrames = 0;
    for (int i = 0; i < enemyRockets.size(); i++)
    {
        enemyRockets[i].update_position();
    }
    // Calc next speed based on position of enemy rocket

    for (int i = 0; i < DEFENDER_ROCKET_NUM; i++)
    {
        DefenderRocket *curr = &defenderRockets[i];
        if (curr->status != FLYING)
            continue;
        int target = curr->rocketTarget;
        if (target == -1)
            continue;
        EnemyRocket target_rocket = enemyRockets[target];
        // If the target is destroyed by another rocket, then disappear
        if (target_rocket.isDestroyed)
        {
            curr->status = HIT_SUCCESS;
            continue;
        }
        double z_diff = target_rocket.pos.z - curr->pos.z;
        double y_diff = target_rocket.pos.y - curr->pos.y;
        double x_diff = target_rocket.pos.x - curr->pos.x;
        double defender_velocity_norma = 40.0;
        // calc sqaure root(size z^2 + y^2 +x^2)
        double diff_norma =
            sqrt(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);

        double ration_to_const_speed = defender_velocity_norma / diff_norma;
        double z_dir = z_diff * ration_to_const_speed;
        double y_dir = y_diff * ration_to_const_speed;
        double x_dir = x_diff * ration_to_const_speed;
        // Updating velocity
        curr->velocity.x = x_dir;
        curr->velocity.y = y_dir;
        curr->velocity.z = z_dir;
        // Updating defender position
        curr->pos.x += (x_dir / 60.0);
        curr->pos.y += (y_dir / 60.0);
        curr->pos.z += (z_dir / 60.0);
    }
}

void reset_game()
{
    enemyRockets.clear();
    defenderRockets.clear();
    buildings.clear();
    // clearing the json buildings
    std::ifstream loadFile("buildings.json");
    json j = json::array();
    loadFile >> j;
    j.clear();
    ofstream newFile("buildings.json");
    newFile << j;
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * ironDomes.size();
         i++)
    {
        // Setting at -1 means they have not been launched yet
        DefenderRocket new_rocket;
        new_rocket.rocketTarget = -1;
        // Each iron dome capsule receives 50 rockets
        int ironDomeId = i / ROCKET_PER_CAPSULE;
        new_rocket.ironDomeId = ironDomeId;
        new_rocket.pos.x = ironDomes[ironDomeId].pos.x;
        new_rocket.pos.y = 1.0 / 2;
        new_rocket.pos.z = ironDomes[ironDomeId].pos.z;
        new_rocket.length = 2;
        defenderRockets.push_back(new_rocket);
    }
}

void add_enemy_rocket()
{
    EnemyRocket new_rocket;
    enemyRockets.push_back(new_rocket);
}

void handle_camera(Camera *camera)
{

    float mouseWheelMovement = GetMouseWheelMove();
    float t = 8.0f;

    if (IsKeyDown(KEY_UP))
    {
        if (!(camera->position.z - t < 150))
        {
            camera->position.z -= t;
            camera->target.z -= t;
        }
    }
    if (IsKeyDown(KEY_DOWN))
    {
        if (!(camera->position.z + t > WORLD_LENGTH / 2 + 100))
        {
            camera->position.z += t;
            camera->target.z += t;
        }
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        camera->position.x += t;
        camera->target.x += t;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        camera->position.x -= t;
        camera->target.x -= t;
    }

    // Zoom in/out - this is for now cancelled, until I figure this shit out
    // if (mouseWheelMovement != 0) {
    //     if (mouseWheelMovement < 0) {
    //         camera->position.y += 10;
    //     } else {
    //         camera->position.y -= 10;
    //     }
    // }
}

void add_building() { isAddingBuilding = true; }

void handle_mouse(Camera &camera)
{
    // I am trying to implement a mouse surround
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !GuiElement::isClickingGuiElement())
    {
        Vector2 mouse_pos = GetMousePosition();
        Ray ray = GetScreenToWorldRay(mouse_pos, camera);
        RayCollision plane_collision = GetRayCollisionBox(
            ray, (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                               {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
        if (!isSurrounding)
        {
            mouse_surround_pos.start.x = plane_collision.point.x;
            mouse_surround_pos.start.y = 0;
            mouse_surround_pos.start.z = plane_collision.point.z;
            mouse_surround_pos.end.x = plane_collision.point.x;
            mouse_surround_pos.end.y = 0;
            mouse_surround_pos.end.z = plane_collision.point.z;
            isSurrounding = true;
        }
        else
        {
            mouse_surround_pos.end.x = plane_collision.point.x;
            mouse_surround_pos.end.y = 0;
            mouse_surround_pos.end.z = plane_collision.point.z;

            // Pick up all the entities that are inside the mouse surround
            for (int i = 0; i < tanks.size(); i++)
            {
                BoundingBox mouse_box = {{min(mouse_surround_pos.start.x, mouse_surround_pos.end.x), min(mouse_surround_pos.start.y, mouse_surround_pos.end.y), min(mouse_surround_pos.start.z, mouse_surround_pos.end.z)}, {max(mouse_surround_pos.start.x, mouse_surround_pos.end.x), max(mouse_surround_pos.start.y, mouse_surround_pos.end.y), max(mouse_surround_pos.start.z, mouse_surround_pos.end.z)}};
                bool is_collision = CheckCollisionBoxes(mouse_box, tanks[i].bounding_box);
                if (is_collision)
                {
                    selectedEntities.clear();
                    selectedEntities.push_back(&tanks[i]);
                }
            }
        }
    }
    else if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && isSurrounding)
    {
        // Now check who is inside the mouse boundaries
        isSurrounding = false;
        // selectedEntities.clear();
    }
    else
    {
        // selectedEntities.clear();
        isSurrounding = false;
    }
}
