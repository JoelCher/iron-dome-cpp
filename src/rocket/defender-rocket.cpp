#include "./defender-rocket.h"
#include "enemy-rocket.h"

void DefenderRocket::updatePosition(EnemyRocket &targetRocket) {
    if (status != FLYING)
        return;
    int target = rocketTarget;
    if (target == -1)
        return;
    // If the target is destroyed by another rocket, then disappear
    if (targetRocket.destroyedPercentage == 100) {
        status = HIT_SUCCESS;
        return;
    }
    double z_diff = targetRocket.pos.z - pos.z;
    double y_diff = targetRocket.pos.y - pos.y;
    double x_diff = targetRocket.pos.x - pos.x;
    // calc sqaure root(size z^2 + y^2 +x^2)
    double diff_norma =
        sqrt(z_diff * z_diff + y_diff * y_diff + z_diff * z_diff);

    double ration_to_const_speed = speed / diff_norma;
    double z_dir = z_diff * ration_to_const_speed;
    double y_dir = y_diff * ration_to_const_speed;
    double x_dir = x_diff * ration_to_const_speed;
    // Updating velocity
    velocity.x = x_dir;
    velocity.y = y_dir;
    velocity.z = z_dir;
    // Updating defender position
    pos.x += (x_dir / 60.0);
    pos.y += (y_dir / 60.0);
    pos.z += (z_dir / 60.0);
}
