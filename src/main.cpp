 #include "../include/raylib.h"
 #include "main.h"
 #include <iostream>
 #include "iron-dome.h"

 int main() {
    // init_window();
    // iron_dome_program();
    std::cout << "Hello there, type a number\n";
    int number;
    std::cin >> number;
    std::cout << "THe number you typed is " << number << std::endl;
    return 0;
}

void init_window() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");
    SetTargetFPS(60);
    // ToggleFullscreen();
}
