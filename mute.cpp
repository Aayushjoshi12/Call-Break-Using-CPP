#include "mute.hpp"

mute::mute(const char *imagepath, Vector2 imagePosition, float size)
{
    Image image = LoadImage(imagepath);
    int origialwidth = image.width;
    int originalheight = image.height;
    int newwidth = static_cast<int>(origialwidth * size);
    int newheight = static_cast<int>(originalheight * size);
    ImageResize(&image, newwidth, newheight);
    text2 = LoadTextureFromImage(image);
    UnloadImage(image);
    pos2 = imagePosition;
}
mute::~mute()
{
    UnloadTexture(text2);
}
void mute::drawm()
{
    DrawTexture(text2, pos2.x, pos2.y, WHITE);
}