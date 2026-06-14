#include <iostream>
#include <raylib.h>
#include "Homedesign.h"

Color border_color_Home = {209, 194, 193, 255};
Color Text_color_Home = {241, 241, 241, 255};
gamestates currentstate = HomeScreen;

Rectangle img_bot_position1 = {160, 310, (float)200, (float)225};
Rectangle img_bot_position2 = {870, 310, (float)220, (float)225};
Rectangle img_bot_border = {150, 310, (float)220, (float)245};
Rectangle img_human_border = {870, 300, (float)220, (float)245};
Rectangle Mute_Button = {(float)(1100.0), (float)70.0, (float)45.0, (float)45.0};
Rectangle UnMute_Button = {(float)(1100.0), (float)70.0, (float)45.0, (float)45.0};

// Loading and Unloading functions
void Home_UI::load()
{ // For load image
    Cinzel_load = LoadFontEx("../Assets/Fonts/Cinzel-Bold.ttf", 256, 0, 0);
    Background_Img_load = LoadTexture("../Assets/Image files/HomeUI.png");
    img_machine_load = LoadTexture("../Assets/Image files/robot-bot-icon.png");
    img_human_load = LoadTexture("../Assets/Image files/human.png");
    img_mute_button_load = LoadTexture("../Assets/Image files/mute.png");
    img_unmute_button_load = LoadTexture("../Assets/Image files/unmute.png");
    mymusic = LoadMusicStream("../Assets/Music/mute.mp3");
}
void Home_UI::unload()
{ // For unloading image
    UnloadTexture(Background_Img_load);
    UnloadFont(Cinzel_load);
    UnloadTexture(img_machine_load);
    UnloadTexture(img_human_load);
    UnloadTexture(img_mute_button_load);
    UnloadTexture(img_unmute_button_load);
    UnloadMusicStream(mymusic);
}

// General Homepage functions

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
{ // Image Function

    DrawTexturePro(
        Background_Img_load,
        (Rectangle){0, 0, (float)Background_Img_load.width, (float)Background_Img_load.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0},
        0.0f,
        WHITE);
}

void Home_UI::check_collision_mute_unmute()
{
    if (CheckCollisionPointRec(GetMousePosition(), Mute_Button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        ismute = !ismute;
    }
    if (ismute)
    {
        SetMusicVolume(mymusic, 0.0f);
    }
    else
    {
        SetMusicVolume(mymusic, 1.0f);
    }
}

void Home_UI::Img_Un_Mute_button(Rectangle load)
{
    if (ismute)
    {
        DrawTexturePro(
            img_mute_button_load,
            (Rectangle){0, 0, (float)img_mute_button_load.width, (float)img_mute_button_load.height},
            load,
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }
    else
    {
        DrawTexturePro(
            img_unmute_button_load,
            (Rectangle){0, 0, (float)img_unmute_button_load.width, (float)img_unmute_button_load.height},
            load,
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }
}
void Home_UI::backGround_Music()
{
    UpdateMusicStream(mymusic);
}

void Home_UI::isclicked()
{
    if (CheckCollisionPointRec(GetMousePosition(), img_bot_border) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        currentstate = PlayBot;
    }
    else if (CheckCollisionPointRec(GetMousePosition(), img_human_border) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        currentstate = PlayHuman;
    }
}

void Home_UI::Homedesign_draw()
{

    Background(); // Image call
    text_CallBreak();
    border_draw(img_bot_border);
    border_draw(img_human_border);
    img_machines(img_bot_position1);
    check_collision_mute_unmute();
    Img_Un_Mute_button(Mute_Button);
    img_human(img_bot_position2);
    isclicked();
    backGround_Music();
}