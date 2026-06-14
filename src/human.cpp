#include "human.hpp"
#include <raylib.h>

// ===========================================================================
// Man
// ===========================================================================

void Man::load(const char *imagepath, Vector2 position, float size)
{
    Image img = LoadImage(imagepath);
    ImageResize(&img,
                static_cast<int>(img.width  * size),
                static_cast<int>(img.height * size));
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
    pos = position;
}

void Man::draw(Color tint)
{
    DrawTexture(texture, (int)pos.x, (int)pos.y, tint);
}

void Man::drawBox(Color borderColor, float thickness)
{
    (void)thickness;
    Rectangle box = { pos.x, pos.y,
                      (float)texture.width, (float)texture.height };
    DrawRectangleRoundedLines(box, 0.15f, 8, borderColor);
}

void Man::unload()
{
    UnloadTexture(texture);
}

// ===========================================================================
// Mute
// ===========================================================================

void Mute::load(const char *pathOff, const char *pathOn,
                Vector2 position, float size)
{
    pos     = position;
    ismuted = false;

    Image img_off = LoadImage(pathOff);
    ImageResize(&img_off,
                static_cast<int>(img_off.width  * size),
                static_cast<int>(img_off.height * size));
    text_off   = LoadTextureFromImage(img_off);
    buttonRect = { pos.x, pos.y,
                   (float)text_off.width, (float)text_off.height };
    UnloadImage(img_off);

    Image img_on = LoadImage(pathOn);
    ImageResize(&img_on,
                static_cast<int>(img_on.width  * size),
                static_cast<int>(img_on.height * size));
    text_on = LoadTextureFromImage(img_on);
    UnloadImage(img_on);
}

void Mute::drawbutton()
{
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        ismuted = !ismuted;
    }

    if (ismuted)
        DrawTexture(text_off, (int)pos.x, (int)pos.y, WHITE);
    else
        DrawTexture(text_on,  (int)pos.x, (int)pos.y, YELLOW);
}

void Mute::unload()
{
    UnloadTexture(text_off);
    UnloadTexture(text_on);
}

// ===========================================================================
// Fonta
// ===========================================================================

void Fonta::load(const char *fontpath, int fontSize)
{
    font = LoadFontEx(fontpath, fontSize, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void Fonta::drawf(const char *text, Vector2 position,
                  float fontSize, float spacing, Color tint)
{
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

void Fonta::unload()
{
    UnloadFont(font);
}

// ===========================================================================
// Game-level objects  (only visible inside human.cpp)
// ===========================================================================

namespace {
    const char *HUMAN_IMG = "C:\\Users\\rajes\\Downloads\\human.png";
    const char *MUTE_OFF  = "C:\\Users\\rajes\\Downloads\\mute.png";
    const char *MUTE_ON   = "C:\\Users\\rajes\\Downloads\\onn (1).png";
    const char *FONT_PATH = "C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf";
    const char *BG_IMG    = "C:\\Users\\rajes\\Downloads\\background (1) (1).png";

    Texture2D cardback;
    Fonta     displayFont;

    Man   player1, player2, player3, player4;
    Mute  muteBtn;
    Fonta font1, font2, font3, font4, font5, font6;
}

// ===========================================================================
// callall  –  load everything once, after InitWindow
// ===========================================================================

void callall()
{
    displayFont.load(FONT_PATH, 48);
    cardback = LoadTexture(BG_IMG);

    player1.load(HUMAN_IMG, { 50,  100 }, 0.8f);
    player2.load(HUMAN_IMG, { 800, 100 }, 0.8f);
    player3.load(HUMAN_IMG, { 50,  400 }, 0.8f);
    player4.load(HUMAN_IMG, { 800, 400 }, 0.8f);

    muteBtn.load(MUTE_OFF, MUTE_ON, { 1100, 20 }, 0.0984f);

    font1.load(FONT_PATH, 48);
    font2.load(FONT_PATH, 48);
    font3.load(FONT_PATH, 48);
    font4.load(FONT_PATH, 48);
    font5.load(FONT_PATH, 48);
    font6.load(FONT_PATH, 48);
}

// ===========================================================================
// callfordrawing  –  draw every frame, NO loading here
// ===========================================================================

void callfordrawing()
{
    float base = (float)displayFont.font.baseSize;

    DrawTexture(cardback, 0, 0, WHITE);
    DrawTextEx(displayFont.font, "Game lobby", { 400, 40 }, base * 1.5f, 2, YELLOW);

    player1.draw();
    player1.drawBox(WHITE, 3.0f);
    font1.drawf("Player 1", { 120, 100 }, base * 0.8f, 2, WHITE);

    player2.draw();
    player2.drawBox(WHITE, 3.0f);
    font2.drawf("Player 2", { 870, 100 }, base * 0.8f, 2, WHITE);

    player3.draw({ 255, 255, 255, 120 });
    player3.drawBox(GRAY, 3.0f);
    font3.drawf("Player 3", { 120, 400 }, base * 0.8f, 2, WHITE);

    player4.draw({ 255, 255, 255, 120 });
    player4.drawBox(GRAY, 3.0f);
    font4.drawf("Player 4", { 870, 400 }, base * 0.8f, 2, WHITE);

    font5.drawf("2/4 have come",         { 450, 650 }, base * 0.8f, 2, WHITE);
    font6.drawf("Waiting for players...", { 300, 700 }, base * 1.2f, 2, YELLOW);

    muteBtn.drawbutton();
}

// ===========================================================================
// unloadall  –  free everything before CloseWindow
// ===========================================================================

void unloadall()
{
    UnloadTexture(cardback);
    displayFont.unload();

    player1.unload(); player2.unload();
    player3.unload(); player4.unload();

    muteBtn.unload();

    font1.unload(); font2.unload(); font3.unload();
    font4.unload(); font5.unload(); font6.unload();
}