#include "font.hpp"

fonta::fonta(const char *fontpath, int fontSize)
{
    font = LoadFontEx(fontpath, fontSize, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}
fonta::~fonta()
{
    UnloadFont(font);
}
void fonta::drawf(const char *text, Vector2 position, float fontSize, float spacing, Color tint){
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}