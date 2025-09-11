#include "./functions.h"

// void Draw3DCircle(Vector3 center, float radius, int segments, Color color,
//                   Vector3 planeNormal) {
//     rlPushMatrix();
//     rlTranslatef(center.x, center.y, center.z);
//
//     // Very simple hack: align to Y axis for XZ plane
//     if (planeNormal.x == 0 && planeNormal.z == 0) {
//         rlRotatef(0, 0, 1, 0);
//     }
//
//     rlBegin(RL_TRIANGLE_FAN);
//     rlColor4ub(color.r, color.g, color.b, color.a);
//     rlVertex3f(0, 0, 0); // center
//
//     for (int i = 0; i <= segments; i++) {
//         float angle = i * 2.0f * PI / segments;
//         float x = radius * cos(angle);
//         float z = radius * sin(angle);
//         rlVertex3f(x, 0, z);
//     }
//     rlEnd();
//     rlPopMatrix();
// }
