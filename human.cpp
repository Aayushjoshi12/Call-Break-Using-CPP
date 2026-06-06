#include "human.hpp"

man::man(const char *imagepath, Vector2 imagePosition, float size)
{
    Image image = LoadImage(imagepath);
    int origialwidth = image.width;
    int originalheight = image.height;
    int newwidth = static_cast<int>(origialwidth * size);
    int newheight = static_cast<int>(originalheight * size);
    ImageResize(&image, newwidth, newheight);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    pos = imagePosition;
}
man::~man()
{
    UnloadTexture(texture);
}
void man::draw(Color tint)
{
    DrawTexture(texture, pos.x, pos.y, tint);
}
void man::drawBox(Color borderColor, float thickness)
{
    Rectangle box = {pos.x, pos.y, (float)texture.width, (float)texture.height};
    DrawRectangleRoundedLines(box, 0.15f, 8, borderColor); //.
}