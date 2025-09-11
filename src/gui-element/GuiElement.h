#pragma once
#include "../../include/raylib.h"
#include <functional>
#include <string>
#include <vector>

class GuiElement {
  public:
    Rectangle pos_rec;
    std::string text;

    GuiElement(Rectangle rec, std::string text,
               const std::function<void(void)> &onClick);

    static std::vector<GuiElement *> elements;
    static bool isClickingGuiElement();

    std::function<void(void)> onClick;
};
