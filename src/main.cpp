#include "main.h"
#include "../include/raylib.h"
#include "iron-dome.h"
#include <iostream>
int main() {
    init_window();
    iron_dome_program();

    return 0;
}

void init_window() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");
    SetTargetFPS(60);
    // ToggleFullscreen();
}
