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


