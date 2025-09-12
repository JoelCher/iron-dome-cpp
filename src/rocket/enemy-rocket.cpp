#include "enemy-rocket.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>

EnemyRocket::EnemyRocket() {
  pos.x = -200;
  pos.y = 100;
  pos.z = (rand() % 501) - 250;
  velocity.x = 30;
  velocity.y = 0;
  velocity.z = (rand() % 100) - 50;

  // Normalize speed
  float norma = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y +
                      velocity.z * velocity.z);
  velocity.x = 20 * velocity.x / norma;
  velocity.y = 20 * velocity.y / norma;
  velocity.z = 20 * velocity.z / norma;

  length = 2;
  isDestroyed = 0;
  destroyedPercentage = 0;
  speed = 20.0f;
};
void EnemyRocket::update_position() {
  // if (destroyedPercentage == 100)
  //     return;

  // The rocket should try to undestand where it might land with the time that
  // it has before it hits the ground, using a simple distance function on the
  // pos.y parameter We can use here the function y= y0 + vt, we have the y,y0
  // and v, and we need to figure out the t. which is here t = (y-y0)/v ,
  // which is basically pos.y/velocity.y
  // Get closer to the target
  // Build the diff between them
  float xDiff = targetPos.x - pos.x;
  float yDiff = targetPos.y - pos.y;
  float zDiff = targetPos.z - pos.z;
  // Now you have the diff vector, and you need to add the diff to the vector,
  // but normalize it to the velocity norma, which means you need to control
  // the speed constant
  float norma = sqrtf(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

  // This isn't great, I need the rocket to be able to slow down and decide
  // what acceleration is necessarry and to which direction it is needed.
  velocity.x += speed * xDiff / norma / 60.0f;
  velocity.y += speed * yDiff / norma / 60.0f;
  velocity.z += speed * zDiff / norma / 60.0f;

  // velocity.y = velocity.y + (1 / 60.0) * -1 * 9.8;
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
