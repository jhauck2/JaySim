#include "raylib.h"
#include <string>
#include <any>

class Button {
public:
    Vector2 position;
    Vector2 size;
    std::string text = "";
    bool highlight = false;
    void (*callback) (std::any);


    Button();
    Button(Vector2 pos, Vector2 size, std::string txt);
    void set_text(std::string t);
    void DrawButton();
    void UpdateButton(std::any var);
    void Click();

};
