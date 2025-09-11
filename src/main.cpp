#include "main.h"
#include "../include/raylib.h"
#include "iron-dome.h"
int main() {
    init_window();
    IronDomeProgram ironDomeProgram;
    ironDomeProgram.init();

    return 0;
}

void init_window() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");
    SetTargetFPS(60);
    // ToggleFullscreen();
}
