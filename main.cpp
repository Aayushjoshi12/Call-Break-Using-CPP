#include <raylib.h>
#include <iostream>
#include "Header folder/Homedesign.h"

using namespace std;
float screenwidth = 1200, screenheight = 800;

Home_UI Home;

int main()
{
    Rectangle img_bot_position1 = {160, 310, (float)200, (float)225};
    Rectangle img_bot_position2 = {870, 310, (float)220, (float)225};
    Rectangle img_bot_border1 = {150, 310, (float)220, (float)245};
    Rectangle img_bot_border2 = {870, 300, (float)220, (float)245};
    Rectangle Mute_Button = {(float)(1100.0), (float)70.0, (float)45.0, (float)45.0};

    InitWindow(screenwidth, screenheight, "Call Break");
    Home.load();
    SetTargetFPS(100);
    // Rectangle button1 = {100, 200, 300, 150};
    // Rectangle button2 = {850, 200, 300, 150};
    while (WindowShouldClose() == false)
    {

        BeginDrawing();

        ClearBackground(RED);
        // Home design
        Home.Background(); // Image call
        Home.text_CallBreak();
        Home.border_draw(img_bot_border1);
        Home.border_draw(img_bot_border2);
        Home.img_machines(img_bot_position1);
        Home.img_human(img_bot_position2);
        Home.Mute_button(Mute_Button);

        EndDrawing();
    }
    Home.unload();
}