#pragma once
#include "./rocket.h"

class EnemyRocket : public Rocket {
  public:
    bool isDestroyed;
    EnemyRocket();
    void update_position();
    Vector3 targetPos;
};
