#include "GuiElement.h"
#include <iostream>

std::vector<GuiElement *> GuiElement::elements;

GuiElement::GuiElement(Rectangle rec) : pos_rec(rec)
{
    elements.push_back(this);
}

bool GuiElement::isClickingGuiElement()
{
    Vector2 mouse_pos = GetMousePosition();
    for (GuiElement *el : GuiElement::elements)
    {
        if (CheckCollisionPointRec(mouse_pos, el->pos_rec))
        {
            return true;
        }
    }
    return false;
}