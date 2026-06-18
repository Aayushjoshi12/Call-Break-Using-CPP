
#include <iostream>
#include <raylib.h>
#include "Homedesign.h"
 
Color border_color_Home = {209, 194, 193, 255};
Color Text_color_Home = {241, 241, 241, 255};
 
Rectangle img_bot_position1 = {160, 310, (float)200, (float)225};
Rectangle img_bot_position2 = {870, 310, (float)220, (float)225};
Rectangle img_bot_border = {150, 310, (float)220, (float)245};
Rectangle img_human_border = {870, 300, (float)220, (float)245};
 
void Home_UI::load()
{
    Cinzel_load = LoadFontEx("../Assets/Fonts/Cinzel-Bold.ttf", 256, 0, 0);
    Background_Img_load = LoadTexture("../Assets/Image files/HomeUI.png");
    img_machine_load = LoadTexture("../Assets/Image files/robot-bot-icon.png");
    img_human_load = LoadTexture("../Assets/Image files/human.png");
}
 
void Home_UI::unload()
{
    UnloadFont(Cinzel_load);
    UnloadTexture(Background_Img_load);
    UnloadTexture(img_machine_load);
    UnloadTexture(img_human_load);
}
 
void Home_UI::text_CallBreak()
{
    DrawTextEx(
        Cinzel_load,
        "Call-Break Game",
        (Vector2){(float)((GetScreenWidth() / 2.0) - 240), 60},
        75,
        1,
        GOLD);
    DrawTextEx(
        Cinzel_load,
        "PLAY WITH BOT",
        (Vector2){(float)160.0, (float)560.0},
        30,
        0.5f,
        Text_color_Home);
    DrawTextEx(
        Cinzel_load,
        "PLAY WITH HUMAN",
        (Vector2){(float)870.0, (float)560.0},
        30,
        0.5f,
        Text_color_Home);
    DrawTextEx(
        Cinzel_load,
        "BY JUWADIS FOR JUWADIS",
        (Vector2){(float)GetScreenWidth() - 400, (float)GetScreenHeight() - 50},
        35,
        0.5f,
        GOLD);
}
 
void Home_UI::border_draw(Rectangle Border)
{
    DrawRectangleRoundedLinesEx(Border, 0.3f, 20, 2, border_color_Home);
}
 
void Home_UI::img_machines(Rectangle Img_bot_machine_position)
{
    DrawTexturePro(
        img_machine_load,
        (Rectangle){0, 0, (float)img_machine_load.width, (float)img_machine_load.height},
        Img_bot_machine_position,
        (Vector2){0, 0},
        0.0f,
        WHITE);
}
 
void Home_UI::img_human(Rectangle Img_bot_machine_position)
{
    DrawTexturePro(
        img_human_load,
        (Rectangle){0, 0, (float)img_human_load.width, (float)img_human_load.height},
        Img_bot_machine_position,
        (Vector2){0, 0},
        0.0f,
        WHITE);
}
 
void Home_UI::Background()
{
    DrawTexturePro(
        Background_Img_load,
        (Rectangle){0, 0, (float)Background_Img_load.width, (float)Background_Img_load.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0},
        0.0f,
        WHITE);
}
 
string Home_UI::isclicked()
{
    if (CheckCollisionPointRec(GetMousePosition(), img_bot_border) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return "Bot";
    else if (CheckCollisionPointRec(GetMousePosition(), img_human_border) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return "Human";
    return "";
}
 
void Home_UI::Homedesign_draw()
{
    Background();
    text_CallBreak();
    border_draw(img_bot_border);
    border_draw(img_human_border);
    img_machines(img_bot_position1);
    img_human(img_bot_position2);
}