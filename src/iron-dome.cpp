#include "./iron-dome.h"
#include "gui-element/GuiElement.h"
#include "rocket/enemy-rocket.h"
#include "rocket/rocket.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

int IronDomeProgram::init(void) {
    GuiElement resetButton({SCREEN_WIDTH - 450, SCREEN_HEIGHT - 100, 120, 30},
                           "reset", [this]() { reset_world(); });
    GuiElement addBuildingButton(
        {SCREEN_WIDTH - 650, SCREEN_HEIGHT - 100, 200, 30}, "Add building",
        [this]() { add_building(); });
    GuiElement shootRocketButton(
        {SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100, 300, 30},
        "Shoot enemty rocket", [this]() { add_enemy_rocket(); });
    guiElements.push_back(resetButton);
    guiElements.push_back(addBuildingButton);
    guiElements.push_back(shootRocketButton);
    // Initializing the defender rockets
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * ironDomes.size();
         i++) {
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

    camera = {0};
    camera.position = (Vector3){0.0f, 300.0f, WORLD_LENGTH / 2.0 + 40};
    camera.target = (Vector3){0.0f, 0.0f, WORLD_LENGTH / 2.0 - 200};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);
    Tank new_tank;
    new_tank.pos = {-200, 10, 100};
    new_tank.bounding_box = {
        .min = {new_tank.pos.x, 0, new_tank.pos.z},
        {new_tank.pos.x, new_tank.pos.y + 10, new_tank.pos.z}};
    tanks.push_back(new_tank);
    while (!WindowShouldClose()) {
        loop();
        draw();
    }

    CloseWindow();
    return 0;
}

void IronDomeProgram::check_rockets_collision() {
    for (int i = 0; i < DEFENDER_ROCKET_NUM; i++) {
        // Don't check for destroyed rockets
        if (defenderRockets[i].status != FLYING)
            continue;
        for (int j = 0; j < enemyRockets.size(); j++) {
            if (enemyRockets[j].isDestroyed)
                continue;
            float z_diff = enemyRockets[j].pos.z - defenderRockets[i].pos.z;
            float y_diff = enemyRockets[j].pos.y - defenderRockets[i].pos.y;
            float x_diff = enemyRockets[j].pos.x - defenderRockets[i].pos.x;
            // calc square root(size z^2 + y^2 +x^2)
            float diff_norma =
                sqrtf(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);

            // For some reason, distance here is different from diff_norma, even
            // though using same functions and methods...
            if (diff_norma < 2) {
                // Stop those two rockets
                enemyRockets[j].isDestroyed = true;
                defenderRockets[i].status = HIT_SUCCESS;
            }
            // TODO: Check collisions with buildings...
        }
    }
}

void IronDomeProgram::update_positions() {
    int animFrames = 0;
    for (int i = 0; i < enemyRockets.size(); i++) {
        enemyRockets[i].update_position();
        if ((enemyRockets[i].isDestroyed || enemyRockets[i].hasFallen()) &&
            enemyRockets[i].destroyedPercentage < 100) {
            enemyRockets[i].destroyedPercentage += 10;
        }
    }

    // Calc next speed based on position of enemy rocket
    for (int i = 0; i < defenderRockets.size(); i++) {
        int target = defenderRockets[i].rocketTarget;
        std::cout << "Target is " << target << std::endl;
        if (target == -1)
            continue;
        EnemyRocket targetRocket = enemyRockets[target];
        defenderRockets[i].updatePosition(targetRocket);
    }
}

void IronDomeProgram::reset_world() {
    enemyRockets.clear();
    defenderRockets.clear();
    buildings.clear();
    for (int i = 0;
         i < DEFENDER_ROCKET_NUM && i < ROCKET_PER_CAPSULE * ironDomes.size();
         i++) {
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

void IronDomeProgram::add_enemy_rocket() {
    EnemyRocket newRocket;
    newRocket.targetPos = (Vector3){300, 0, 0};
    enemyRockets.push_back(newRocket);
    // Increase wasted cash
    moneyWasted += 20000;
}

void IronDomeProgram::handle_camera(Camera *camera) {
    float mouseWheelMovement = GetMouseWheelMove();
    float t = 8.0f;

    if (IsKeyDown(KEY_UP)) {
        if (!(camera->position.z - t < 150)) {
            camera->position.z -= t;
            camera->target.z -= t;
        }
    }
    if (IsKeyDown(KEY_DOWN)) {
        if (!(camera->position.z + t > WORLD_LENGTH / 2 + 100)) {
            camera->position.z += t;
            camera->target.z += t;
        }
    }
    if (IsKeyDown(KEY_RIGHT)) {
        camera->position.x += t;
        camera->target.x += t;
    }
    if (IsKeyDown(KEY_LEFT)) {
        camera->position.x -= t;
        camera->target.x -= t;
    }
}

void IronDomeProgram::add_building() { isAddingBuilding = true; }

void IronDomeProgram::handle_mouse(Camera *camera) {
    // Handle pov changing
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mouseDelta = GetMouseDelta();
        // Move the target of the camera based on mouse movement now
        camera->target.x += mouseDelta.x;
        camera->target.y -= mouseDelta.y;
    }

    // Handle mouse scroll wheel
    int mouseWheelMove = GetMouseWheelMove();
    if (mouseWheelMove > 0) {
        camera->position.y -= 20;
    } else if (mouseWheelMove < 0) {
        camera->position.y += 20;
    }
    // I am trying to implement a mouse surround
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
        !GuiElement::isClickingGuiElement()) {
        Vector2 mouse_pos = GetMousePosition();
        Ray ray = GetScreenToWorldRay(mouse_pos, *camera);
        RayCollision plane_collision = GetRayCollisionBox(
            ray, (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                               {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
        if (!isSurrounding) {
            mouse_surround_pos.start.x = plane_collision.point.x;
            mouse_surround_pos.start.y = 0;
            mouse_surround_pos.start.z = plane_collision.point.z;
            mouse_surround_pos.end.x = plane_collision.point.x;
            mouse_surround_pos.end.y = 0;
            mouse_surround_pos.end.z = plane_collision.point.z;
            isSurrounding = true;
        } else {
            mouse_surround_pos.end.x = plane_collision.point.x;
            mouse_surround_pos.end.y = 0;
            mouse_surround_pos.end.z = plane_collision.point.z;

            // Pick up all the entities that are inside the mouse surround
            for (int i = 0; i < tanks.size(); i++) {
                BoundingBox mouse_box = {{std::min(mouse_surround_pos.start.x,
                                                   mouse_surround_pos.end.x),
                                          std::min(mouse_surround_pos.start.y,
                                                   mouse_surround_pos.end.y),
                                          std::min(mouse_surround_pos.start.z,
                                                   mouse_surround_pos.end.z)},
                                         {std::max(mouse_surround_pos.start.x,
                                                   mouse_surround_pos.end.x),
                                          std::max(mouse_surround_pos.start.y,
                                                   mouse_surround_pos.end.y),
                                          std::max(mouse_surround_pos.start.z,
                                                   mouse_surround_pos.end.z)}};
                bool is_collision =
                    CheckCollisionBoxes(mouse_box, tanks[i].bounding_box);
                if (is_collision) {
                    selectedEntities.clear();
                    selectedEntities.push_back(&tanks[i]);
                }
            }
        }
    } else if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && isSurrounding) {
        // Now check who is inside the mouse boundaries
        isSurrounding = false;
        // selectedEntities.clear();
    } else {
        // selectedEntities.clear();
        isSurrounding = false;
    }
}

void IronDomeProgram::loop() {
    handle_camera(&camera);
    handle_mouse(&camera);
    // Assigning targets to defender rockets
    // We need to check if there are any enemy rockets
    if (enemyRockets.size()) {
        for (int i = 0; i < enemyRockets.size(); i++) {
            // Check if there is already a defender rocket assigned to this
            // rocket
            bool is_already_assigned = false;
            for (int j = 0; j < DEFENDER_ROCKET_NUM; j++) {
                if (defenderRockets[j].rocketTarget == i)
                    is_already_assigned = true;
            }
            if (!is_already_assigned) {
                // Then assign some defender rocket to this enemy rocket
                // Check which capsule is closest to that rocket
                EnemyRocket curr_enemy_rocket = enemyRockets[i];
                int closest_capsule = 0;
                double closest_distance = -1;
                for (int z = 0; z < ironDomes.size(); z++) {
                    Capsule curr_capsule = ironDomes[z];
                    double x_diff =
                        curr_enemy_rocket.pos.x - curr_capsule.pos.x;
                    double y_diff =
                        curr_enemy_rocket.pos.y - curr_capsule.pos.y;
                    double z_diff =
                        curr_enemy_rocket.pos.z - curr_capsule.pos.z;
                    double distance = sqrt(x_diff * x_diff + y_diff * y_diff +
                                           z_diff * z_diff);
                    if (distance < closest_distance || closest_distance == -1) {
                        closest_distance = distance;
                        closest_capsule = z;
                    }
                }
                for (int j = 0; j < DEFENDER_ROCKET_NUM; j++) {
                    if (defenderRockets[j].ironDomeId == closest_capsule &&
                        defenderRockets[j].rocketTarget == -1) {
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
}

void IronDomeProgram::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    // Draw around all selected entities
    for (const Tank *entity : selectedEntities) {
        DrawBoundingBox(entity->bounding_box, RED);
    }
    // Drawing a floor
    DrawPlane((Vector3){0, 0, 0}, (Vector2){WORLD_WIDTH, WORLD_LENGTH},
              LIGHTGRAY);
    // Drawing the iron dome capsules
    for (int i = 0; i < ironDomes.size(); i++) {
        DrawSphere(ironDomes[i].pos, 5, BLUE);
    }
    // Drawing the enemy rocket
    for (int i = 0; i < enemyRockets.size(); i++) {
        EnemyRocket curr_enemy_rocket = enemyRockets[i];
        if (enemyRockets[i].destroyedPercentage == 100 ||
            curr_enemy_rocket.hasFallen())
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
        // Now take in effect the process of being destroyed
        float percentage =
            (100 - curr_enemy_rocket.destroyedPercentage) / 100.0f;
        d = d / ((100 - curr_enemy_rocket.destroyedPercentage) / 100.0f);

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
        // Draw the target of the enemy rocket
        float arrivalTime =
            std::abs(curr_enemy_rocket.pos.y /
                     curr_enemy_rocket.velocity.y); // In what units is this?
        // Now that we have the time, we can figure out the x position that it
        // will arrive to, based on the params that we currently have We will
        // use the function x = velocity.x * t
        float finalX = curr_enemy_rocket.velocity.x * arrivalTime +
                       curr_enemy_rocket.pos.x;
        float finalZ = curr_enemy_rocket.velocity.z * arrivalTime +
                       curr_enemy_rocket.pos.z;
        DrawPlane(curr_enemy_rocket.targetPos, (Vector2){10, 10}, RED);
        DrawPlane((Vector3){finalX, 0, finalZ}, (Vector2){10, 10}, RED);
    }

    for (int i = 0; i < defenderRockets.size(); i++) {
        if (defenderRockets[i].rocketTarget != -1 &&
            defenderRockets[i].status == FLYING) {
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

    if (has_selected_target_position) {
        for (Tank *tank : selectedEntities) {
            // Move to that direction
            // I need to shrink the distance vector to the speed of the
            // tank(which is tank.speed)
            Vector3 distance_v = {selectedEntitiesTarget.x - tank->pos.x, 0,
                                  selectedEntitiesTarget.z - tank->pos.z};
            float distance_norm =
                sqrt(distance_v.x * distance_v.x + distance_v.z * distance_v.z);
            if (distance_norm < 1)
                continue;
            float x_dir = distance_v.x * (tank->speed / distance_norm);
            float z_dir = distance_v.z * (tank->speed / distance_norm);
            tank->pos.z += z_dir / 60.0;
            tank->pos.x += x_dir / 60.0;
        }
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        !GuiElement::isClickingGuiElement()) {
        // Translating the mouse click to the 3d platform
        Vector2 mouse_pos = GetMousePosition();
        Ray ray = GetMouseRay(mouse_pos, camera);
        RayCollision real_mouse_pos = GetRayCollisionBox(
            ray, (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                               {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
        if (selectedEntities.size()) {
            selectedEntitiesTarget = real_mouse_pos.point;
            has_selected_target_position = true;
        }
    }
    if (isAddingBuilding) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            !GuiElement::isClickingGuiElement()) {
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

        } else {
            // Draw a building following the mouse
            Vector2 mouse_pos = GetMousePosition();
            Ray pos = GetMouseRay(mouse_pos, camera);
            RayCollision collision = GetRayCollisionBox(
                pos, (BoundingBox){{WORLD_WIDTH / 2, 0, WORLD_LENGTH / 2},
                                   {-WORLD_WIDTH / 2, 0, -WORLD_LENGTH / 2}});
            newBuilding.pos.x = collision.point.x;
            newBuilding.pos.z = collision.point.z;
            DrawCubeV(newBuilding.pos, newBuilding.size, newBuilding.color);
        }
    }
    // Just drawing a bunch of buildings
    for (int i = 0; i < buildings.size(); i++) {
        DrawCubeV(buildings[i].pos, buildings[i].size, buildings[i].color);
    }

    DrawLine3D((Vector3){0, 0, -WORLD_LENGTH / 2},
               (Vector3){0, 0, WORLD_LENGTH / 2}, RED);

    if (isSurrounding) {
        const float size_x =
            (mouse_surround_pos.start.x - mouse_surround_pos.end.x);
        const float size_z =
            (mouse_surround_pos.start.z - mouse_surround_pos.end.z);
        DrawCubeWiresV((Vector3){mouse_surround_pos.start.x - size_x / 2.0f, 10,
                                 mouse_surround_pos.start.z - size_z / 2.0f},
                       (Vector3){abs(size_x), 0, abs(size_z)}, GREEN);
    }
    EndMode3D();

    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    for (const GuiElement guiEl : guiElements) {
        if (GuiButton(guiEl.pos_rec, guiEl.text.c_str())) {
            guiEl.onClick();
        }
    }
    // std::string text = std::to_string(moneyWasted);
    // GuiTextBox((Rectangle){100, 100, 100, 100}, text.data(), 20, false);
    EndDrawing();
}

void IronDomeProgram::escapeDefenderRockets() {
    // The purpose here is to detect, for each enemy rocket, which defender
    // rocket is coming at it
    // There are many ways to tackle this problem. One of them that I can think
    // of is to just calculate the distance between all the defender rockets,
    // and then if one of them is really close, run away from their position, by
    // shifting the velocity side ways. The things is, the rocket has to
    // remember what it's target is.
}

float getDistance(Vector3 v1, Vector3 v2) {
    float z_diff = v1.z - v2.z;
    float y_diff = v1.y - v2.y;
    float x_diff = v1.x - v2.x;
    // calc square root(size z^2 + y^2 +x^2)
    float diff_norma =
        sqrtf(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);
    return diff_norma;
}
