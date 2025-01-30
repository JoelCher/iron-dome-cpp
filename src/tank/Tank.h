#include "../../include/raylib.h"
#include "../entity/Entity.h"
class Tank : public Entity
{
public:
  BoundingBox bounding_box;
  Tank();
};