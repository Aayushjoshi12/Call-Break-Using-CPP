#pragma once
#include <raylib.h>
class man
{
public:
    man(const char *imagepath, Vector2 imagePosition, float size);
    ~man();
    void draw(Color tint = WHITE);
    void drawBox(Color borderColor, float thickness = 3.0f); //.
private:
    Texture2D texture;
    Vector2 pos;
};
