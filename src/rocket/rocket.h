#pragma once
#include "../../include/raylib.h"
#include "../entity/Entity.h"

class Rocket : public Entity {
  public:
    Vector3 pos;
    Vector3 velocity;
    int length;
    int destroyedPercentage;
    bool hasFallen();
};

class DefenderRocket : public Rocket {
  public:
    int ironDomeId;
    int rocketTarget;
    int status;
    float speed = 40.0f;
    Vector3 targetPosition;
};
