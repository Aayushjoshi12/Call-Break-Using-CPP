#include <raylib.h>
#include <iostream>
#include "Header folder/Homedesign.h"

using namespace std;
float screenwidth = 1200, screenheight = 800;

Home_UI Home;

int main()
{
    Rectangle img_bot_position1 = {150, 300, (float)200, (float)225};
    Rectangle img_bot_position2 = {870, 300, (float)200, (float)225};
    Rectangle img_bot_border1 = {150, 300, (float)200, (float)225};
    Rectangle img_bot_border2 = {870, 300, (float)200, (float)225};
    
    InitWindow(screenwidth, screenheight, "Call Break");
    Home.load();
    SetTargetFPS(100);
    // Rectangle button1 = {100, 200, 300, 150};
    // Rectangle button2 = {850, 200, 300, 150};
    while (WindowShouldClose() == false)
    {

        BeginDrawing();

        ClearBackground(RED);
        Home.Background(); // Image call
        Home.text_CallBreak();
        Home.border_draw(img_bot_border1);
        Home.border_draw(img_bot_border2);
        Home.img_machines(img_bot_position1);
        Home.img_machines(img_bot_position2);

        // Home.button_draw(100.0, 200.0);
        //  Home.button_draw(850.0, 200.0);
        // if (
        //     CheckCollisionPointRec(GetMousePosition(), button1) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        // {
        //     DrawRectangleRec(button1, RED);
        // }
        // else
        // {
        //     DrawRectangleRec(button1, GOLD);
        // }

        EndDrawing();
    }
    Home.unload();
}