#pragma once
#include "../main.h"
#include "./rocket.h"
#include "enemy-rocket.h"

class DefenderRocket : public Rocket {
  public:
    int ironDomeId;
    int rocketTarget;
    int status;
    float speed = 40.0f;
    Vector3 targetPosition;
    void updatePosition(EnemyRocket &enemyRocket);
};
