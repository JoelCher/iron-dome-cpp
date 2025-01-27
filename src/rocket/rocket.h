#pragma once
#include "../../include/raylib.h"

class Rocket{
  public: 
    Vector3 pos;
    Vector3 velocity;
    int length;
};

class EnemyRocket : public Rocket{
  public:
    bool is_destroyed;
    EnemyRocket();
    void update_position();
};
class DefenderRocket: public Rocket{
  public:
    int iron_dome_id;
    int rocket_target;
    int status;
};
