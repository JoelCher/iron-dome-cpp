#include "./iron-dome.h"
#include "../include/json.hpp"
#include "rocket/rocket.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
using json = nlohmann::json;
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
using namespace std;

vector<DefenderRocket> defender_rockets;
vector<EnemyRocket> enemy_rockets;

vector<Capsule> iron_domes = {
    {{40, 0, -200}, 50}, {{40, 0, 200}, 50}, {{40, 0, 0}, 50}};
Building new_building = {{-100, 50.0 / 2, 30}, {3, 50, 5}, WHITE};
bool is_adding_building = false;
vector<Building> buildings = {{{20, 10.0 / 2, 0}, {2, 10, 3}, WHITE},
                              {{25, 15.0 / 2, 5}, {1.5, 15, 3}, WHITE},
                              {{44, 20.0 / 2, 10}, {2, 20, 5}, WHITE},
                              {{30, 10.0 / 2, 20}, {1.5, 10, 3}, WHITE},
                              {{100, 50.0 / 2, 30}, {3, 50, 5}, WHITE},
                              {{-10, 1.0 / 2, -5}, {1, 1, 1}, WHITE}};

int iron_dome_program(void) {
    // Loading json data about buildings
    std::ifstream loadFile("buildings.json");
    if (loadFile.is_open()) {
        json loadedData;
        loadFile >> loadedData; // Load JSON from file
        loadFile.close();
        for (int i = 0; i < loadedData.size(); i++) {
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
        // Access nested data

        // Print data
    } else {
        cerr << "Failed to open the file for reading." << std::endl;
    }
    // Initializing the defender rockets
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * iron_domes.size();
         i++) {
        // Setting at -1 means they have not been launched yet
        DefenderRocket new_rocket;
        new_rocket.rocket_target = -1;
        // Each iron dome capsule receives 50 rockets
        int iron_dome_id = i / ROCKET_PER_CAPSULE;
        new_rocket.iron_dome_id = iron_dome_id;
        new_rocket.pos.x = iron_domes[iron_dome_id].pos.x;
        new_rocket.pos.y = 1.0 / 2;
        new_rocket.pos.z = iron_domes[iron_dome_id].pos.z;
        new_rocket.length = 2;
        defender_rockets.push_back(new_rocket);
    }

    // Define the camera to look into our 3d world
    // ChangeDirectory(GetApplicationDirectory());
    Camera camera = {0};
    camera.position =
        (Vector3){0.0f, 100.0f, WORLD_LENGTH / 2.0 + 40}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f,
                          0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;         // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // BoundingBox rocket_bounds = GetMeshBoundingBox(rocket_model.meshes[0]);
    // Main game loop
    double start_of_game = GetTime();
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        handle_camera(&camera);

        // Assigning targets to defender rockets
        // We need to check if there are any enemy rockets
        if (enemy_rockets.size()) {
            for (int i = 0; i < enemy_rockets.size(); i++) {
                // Check if there is already a defender rocket assigned to this
                // rocket
                bool is_already_assigned = false;
                for (int j = 0; j < DEFENDER_ROCKET_NUM; j++) {
                    if (defender_rockets[j].rocket_target == i)
                        is_already_assigned = true;
                }
                if (!is_already_assigned) {
                    // Then assign some defender rocket to this enemy rocket
                    // Check which capsule is closest to that rocket
                    EnemyRocket curr_enemy_rocket = enemy_rockets[i];
                    int closest_capsule = 0;
                    double closest_distance = -1;
                    for (int z = 0; z < iron_domes.size(); z++) {
                        Capsule curr_capsule = iron_domes[z];
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
                            closest_distance == -1) {
                            closest_distance = distance;
                            closest_capsule = z;
                        }
                    }
                    for (int j = 0; j < DEFENDER_ROCKET_NUM; j++) {
                        if (defender_rockets[j].iron_dome_id ==
                                closest_capsule &&
                            defender_rockets[j].rocket_target == -1) {
                            printf("The closest capsule is %d\n",
                                   closest_capsule);
                            defender_rockets[j].rocket_target = i;
                            defender_rockets[j].status = FLYING;
                            iron_domes[defender_rockets[j].iron_dome_id]
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

        // Drawing a floor
        DrawPlane((Vector3){0, 0, 0}, (Vector2){WORLD_WIDTH, WORLD_LENGTH},
                  LIGHTGRAY);
        // Drawing the iron dome capsules
        for (int i = 0; i < iron_domes.size(); i++) {
            DrawSphere(iron_domes[i].pos, 5, BLUE);
        }
        // Drawing the enemy rocket
        for (int i = 0; i < enemy_rockets.size(); i++) {
            if (enemy_rockets[i].is_destroyed)
                continue;
            // DrawCubeV(enemy_rockets[i].pos, enemy_rockets[i].size, RED);
            // DrawCubeWiresV(enemy_rockets[i].pos, enemy_rockets[i].size, RED);
            Vector3 curr_pos = enemy_rockets[i].pos;
            Vector3 curr_velocity = enemy_rockets[i].velocity;
            // Calc the velocity norm
            double speed = sqrt(curr_velocity.x * curr_velocity.x +
                                curr_velocity.y * curr_velocity.y +
                                curr_velocity.z * curr_velocity.z);
            // The size of the rocket is 2, so I need to find the number to
            // divide bla bla bla(I dont know how to explain, im stupid)
            float d = speed / enemy_rockets[i].length;
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

        for (int i = 0; i < defender_rockets.size(); i++) {
            if (defender_rockets[i].rocket_target != -1 &&
                defender_rockets[i].status == FLYING) {
                Vector3 curr_pos = defender_rockets[i].pos;
                Vector3 curr_velocity = defender_rockets[i].velocity;
                double speed = sqrt(curr_velocity.x * curr_velocity.x +
                                    curr_velocity.y * curr_velocity.y +
                                    curr_velocity.z * curr_velocity.z);
                float d = speed / defender_rockets[i].length;
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

        if (is_adding_building) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Place the building
                Building building_to_add;
                building_to_add.pos.x = new_building.pos.x;
                building_to_add.pos.y = new_building.pos.y;
                building_to_add.pos.z = new_building.pos.z;
                building_to_add.size.x = new_building.size.x;
                building_to_add.size.y = new_building.size.y;
                building_to_add.size.z = new_building.size.z;
                building_to_add.color = new_building.color;
                buildings.push_back(building_to_add);
                is_adding_building = false;
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
            } else {
                // Draw a building following the mouse
                Vector2 mouse_pos = GetMousePosition();
                Ray pos = GetMouseRay(mouse_pos, camera);
                RayCollision collision = GetRayCollisionBox(
                    pos,
                    (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                                  {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
                new_building.pos.x = collision.point.x;
                new_building.pos.z = collision.point.z;
                DrawCubeV(new_building.pos, new_building.size,
                          new_building.color);
            }
        }
        // Just drawing a bunch of buildings
        for (int i = 0; i < buildings.size(); i++) {
            DrawCubeV(buildings[i].pos, buildings[i].size, buildings[i].color);
        }

        DrawLine3D((Vector3){-WORLD_WIDTH / 2 + 50, 0, -WORLD_LENGTH / 2},
                   (Vector3){-WORLD_WIDTH / 2 + 50, 0, WORLD_LENGTH / 2}, RED);

        EndMode3D();

        GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        if (GuiButton(
                (Rectangle){SCREEN_WIDTH - 450, SCREEN_HEIGHT - 100, 120, 30},
                "Reset")) {
            reset_game();
        }
        if (GuiButton(
                (Rectangle){SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100, 300, 30},
                "Shoot enemy rocket")) {
            add_enemy_rocket();
        }
        if (GuiButton(
                (Rectangle){SCREEN_WIDTH - 650, SCREEN_HEIGHT - 100, 200, 30},
                "Add building")) {
            add_building();
        }
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // UnloadModel(rocket_model); // Unload model
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void check_rockets_collision() {
    for (int i = 0; i < DEFENDER_ROCKET_NUM; i++) {
        // Dont check for destroyed rockets
        if (defender_rockets[i].status != FLYING)
            continue;
        for (int j = 0; j < enemy_rockets.size(); j++) {
            if (enemy_rockets[j].is_destroyed)
                continue;
            double z_diff = enemy_rockets[j].pos.z - defender_rockets[i].pos.z;
            double y_diff = enemy_rockets[j].pos.y - defender_rockets[i].pos.y;
            double x_diff = enemy_rockets[j].pos.x - defender_rockets[i].pos.x;
            // calc square root(size z^2 + y^2 +x^2)
            double diff_norma =
                sqrt(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);
            if (diff_norma < 2) {
                // Stop those two rockets
                enemy_rockets[j].is_destroyed = true;
                defender_rockets[i].status = HIT_SUCCESS;
            }
        }
    }
}

void update_positions() {
    int animFrames = 0;
    for (int i = 0; i < enemy_rockets.size(); i++) {
        enemy_rockets[i].update_position();
    }
    // Calc next speed based on position of enemy rocket

    for (int i = 0; i < DEFENDER_ROCKET_NUM; i++) {
        DefenderRocket *curr = &defender_rockets[i];
        if (curr->status != FLYING)
            continue;
        int target = curr->rocket_target;
        if (target == -1)
            continue;
        EnemyRocket target_rocket = enemy_rockets[target];
        // If the target is destroyed by another rocket, then disappear
        if (target_rocket.is_destroyed) {
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

        double ration_to_const_speed = diff_norma / defender_velocity_norma;
        double z_dir = z_diff / ration_to_const_speed;
        double y_dir = y_diff / ration_to_const_speed;
        double x_dir = x_diff / ration_to_const_speed;
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

void reset_game() {
    enemy_rockets.clear();
    defender_rockets.clear();
    buildings.clear();
    // clearing the json buildings
    std::ifstream loadFile("buildings.json");
    json j = json::array();
    loadFile >> j;
    j.clear();
    ofstream newFile("buildings.json");
    newFile << j;
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * iron_domes.size();
         i++) {
        // Setting at -1 means they have not been launched yet
        DefenderRocket new_rocket;
        new_rocket.rocket_target = -1;
        // Each iron dome capsule receives 50 rockets
        int iron_dome_id = i / ROCKET_PER_CAPSULE;
        new_rocket.iron_dome_id = iron_dome_id;
        new_rocket.pos.x = iron_domes[iron_dome_id].pos.x;
        new_rocket.pos.y = 1.0 / 2;
        new_rocket.pos.z = iron_domes[iron_dome_id].pos.z;
        new_rocket.length = 2;
        defender_rockets.push_back(new_rocket);
    }
}

void add_enemy_rocket() {
    EnemyRocket new_rocket;
    enemy_rockets.push_back(new_rocket);
}

void handle_camera(Camera *camera) {
    // if (IsCursorHidden())

    float mouseWheelMovement = GetMouseWheelMove();
    float t = 4.0f;

    if (IsKeyDown(KEY_UP)) {
        camera->position.y += t;
    }
    if (IsKeyDown(KEY_DOWN)) {
        camera->position.y -= t;
    }

    // Calculate movement vector
    float tx = camera->position.z;
    float tz = camera->position.x;
    float length = sqrt(tx * tx + tz * tz);
    tx = (tx / length) * t;
    tz = (tz / length) * t;

    if (IsKeyDown(KEY_RIGHT)) {
        camera->position.x += tx;
        camera->position.z -= tz;
    }
    if (IsKeyDown(KEY_LEFT)) {
        camera->position.x -= tx;
        camera->position.z += tz;
    }

    // Zoom in/out
    if (mouseWheelMovement != 0) {
        float rx = camera->position.x;
        float rz = camera->position.z;
        float ry = camera->position.y;

        length = sqrt(rx * rx + ry * ry + rz * rz);

        rx = (rx / length) * t;
        rz = (rz / length) * t;
        ry = (ry / length) * t;

        if (mouseWheelMovement < 0) {
            camera->position.x += rx;
            camera->position.z += rz;
            camera->position.y += ry;
        } else {
            camera->position.x -= rx;
            camera->position.z -= rz;
            camera->position.y -= ry;
        }
    }
    // Toggle camera controls
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        UpdateCamera(camera, CAMERA_FIRST_PERSON);
    } else {
        UpdateCamera(camera, CAMERA_PERSPECTIVE);
    }
}

void add_building() { is_adding_building = true; }
