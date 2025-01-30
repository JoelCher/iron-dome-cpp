#pragma once
#include "../../include/raylib.h"
#include "../entity/Entity.h"

class Rocket : public Entity
{
public:
  Vector3 pos;
  Vector3 velocity;
  int length;
};

class EnemyRocket : public Rocket
{
public:
  bool isDestroyed;
  EnemyRocket();
  void update_position();
};
class DefenderRocket : public Rocket
{
public:
  int ironDomeId;
  int rocketTarget;
  int status;
};
