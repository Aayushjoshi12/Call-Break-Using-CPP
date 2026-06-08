#include <iostream>
#include <raylib.h>
#include "RendererInterface.h"
using namespace std;

int main()
{
    int sh = 800;
    int sw = 1200;
    InitWindow(sw, sh, "Call Break");
    Renderer renderer;
    Font font = LoadFontEx("../Assets/fonts/Cinzel.ttf", 96, NULL, 0);
    Font boldFont = LoadFontEx("../Assets/fonts/Cinzel-Bold.ttf", 96, NULL, 0);

    Color goldColor = (Color){201, 168, 76, 255};

    while (!WindowShouldClose())
    {
        BeginDrawing();
         ClearBackground(RAYWHITE);
         renderer.drawWholeInterface();
         EndDrawing();
    }

    CloseWindow();
    return 0;
}