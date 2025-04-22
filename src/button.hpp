#include "raylib.h"
#include <string>
#include <any>
#include <optional>

class Button {
public:
    Vector2 position;
    Vector2 size;
    std::string text = "";
    void (*callback) (std::any);


    Button();
    Button(Vector2 pos, Vector2 size, std::string txt);
    void set_text(std::string t);
    void DrawButton();
    void Click();

};
