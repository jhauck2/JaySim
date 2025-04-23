#include "button.hpp"
#include <stdio.h>


Button::Button(){}

Button::Button(Vector2 pos, Vector2 size, std::string txt) {
    this->position = pos;
    this->size = size;
    this->text = std::move(txt);
}
void Button::set_text(std::string t) {
    this->text = t;
}

void Button::DrawButton() {
    DrawRectangleV(this->position, this->size, LIGHTGRAY);
    DrawRectangleLines(this->position.x, this->position.y, this->size.x, this->size.y, GRAY);
    DrawText(this->text.c_str(), (int)(this->position.x+this->size.x/2 - MeasureText(this->text.c_str(), 12)/2), (int)(this->position.y+this->size.y/2 -6), 12, BLACK);
}

void Button::Click(){
    // printf("Click! %s\n", this->text.c_str());
}
