#include <raylib.h>
#include <iostream>
#include "Header folder/Homedesign.h"

using namespace std;
float screenwidth = 1200, screenheight = 800;

Home_UI Home;

int main()
{
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
        Home.text_CallBreak();

        EndDrawing();
    }
    Home.unload();
}