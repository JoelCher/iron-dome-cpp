#pragma once
#include "../../include/raylib.h"
#include <vector>

class GuiElement
{
public:
    Rectangle pos_rec;

    GuiElement(Rectangle rec);

    static std::vector<GuiElement *> elements;
    static bool isClickingGuiElement();
};