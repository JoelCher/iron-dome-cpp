#include "rocket.h"
#include "../iron-dome.h"
#include <random>

// void add_enemy_rocket() {
//     enemy_rockets_count++;
//     EnemyRocket *new_rocket = &enemy_rockets[enemy_rockets_count - 1];
//     new_rocket->pos.x = -WORLD_WIDTH / 2 + 50;
//     new_rocket->pos.y = 100;
//     new_rocket->pos.z = (rand() % 501) - 250;
//     new_rocket->velocity.x = 30;
//     new_rocket->velocity.y = 5;
//     int random_z_speed = (rand() % 100) - 50;
//     new_rocket->velocity.z = random_z_speed;
//     new_rocket->length = 2;
//
// }
//
EnemyRocket::EnemyRocket() {
    pos.x = -WORLD_WIDTH / 2 + 50;
    pos.y = 100;
    pos.z = (rand() % 501) - 250;
    velocity.x = 30;
    velocity.y = 5;
    int random_z_speed = (rand() % 100) - 50;
    velocity.z = random_z_speed;
    length = 2;
};
void EnemyRocket::update_position() {
    if (is_destroyed)
        return;
    velocity.y = velocity.y + (1 / 60.0) * -1 * 9.8;
    double new_enemy_pos_y = pos.y + (velocity.y / 60.0);
    // stop if hitting the ground
    if (new_enemy_pos_y <= 1.0 / 2) {
        velocity.y = (velocity.y = 0);
    } else {
        pos.x = pos.x + (velocity.x / 60.0);
        pos.y = new_enemy_pos_y;
        pos.z = pos.z + (velocity.z / 60.0);
    }
}
