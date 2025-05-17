#include "button.hpp"
#include <raylib.h>



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
    Color box_color = RAYWHITE;
    if (highlight) box_color = SKYBLUE;
    DrawRectangleV(this->position, this->size, box_color);
    DrawRectangleLines(this->position.x, this->position.y, this->size.x, this->size.y, GRAY);
    DrawText(this->text.c_str(), (int)(this->position.x+this->size.x/2 - MeasureText(this->text.c_str(), 12)/2), (int)(this->position.y+this->size.y/2 -6), 12, BLACK);
}

void Button::UpdateButton(std::any var) {
    // Handle button clicks
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, (Rectangle){position.x, position.y, size.x, size.y})) {
        // Set highlight
        highlight = true;
        // Check click
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Click();
            (*(callback))(var);
        }
    }
    else highlight = false;
}

void Button::Click(){
    // printf("Click! %s\n", this->text.c_str());
}
