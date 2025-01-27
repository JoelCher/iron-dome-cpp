 #include "../include/raylib.h"
 #include "main.h"
 #include <iostream>

 int main(){
    // InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");
    SetTargetFPS(60);    
    // while(!WindowShouldClose()){
      std:: cout << "HEY there\n";
      int input=std::cin.get();
      std:: cout<< input << std::endl;
    // }
  return 0;
 }