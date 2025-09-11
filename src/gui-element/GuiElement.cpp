#include "GuiElement.h"
#include <iostream>

std::vector<GuiElement *> GuiElement::elements;

GuiElement::GuiElement(Rectangle rec, std::string text,
                       const std::function<void(void)> &onClick)
    : pos_rec(rec), text(text), onClick(onClick) {
    elements.push_back(this);
}

bool GuiElement::isClickingGuiElement() {
    Vector2 mouse_pos = GetMousePosition();
    for (GuiElement *el : GuiElement::elements) {
        if (CheckCollisionPointRec(mouse_pos, el->pos_rec)) {
            return true;
        }
    }
    return false;
}
