#include <iostream>
#include <raylib.h>
#include "Renderer.h"
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
        renderer.drawBackground();

        for (int i = 0; i < 3; i++)
        {
            switch (i)
            {
            case 0:
            {
                renderer.drawCardBack(600.0, 100.0);
                Vector2 size = MeasureTextEx(font, "Bot 1", 48, 2);
                float x = 600.0 - size.x / 2;
                float y = 100.0 + 108.72 - size.y - 10;
                DrawTextEx(font, "Bot 1", (Vector2){x, y}, 48, 2, WHITE);
                break;
            }
            case 1:
            {
                renderer.drawCardBack(200.0, 400.0);
                Vector2 size = MeasureTextEx(font, "Bot 2", 48, 2);
                float x = 200.0 - size.x / 2;
                float y = 400.0 + 108.72 - size.y - 10;
                DrawTextEx(font, "Bot 2", (Vector2){x, y}, 48, 2, WHITE);
                break;
            }
            case 2:
            {
                renderer.drawCardBack(1000.0, 400.0);
                Vector2 size = MeasureTextEx(font, "Bot 3", 48, 2);
                float x = 1000.0 - size.x / 2;
                float y = 400.0 + 108.72 - size.y - 10;
                DrawTextEx(font, "Bot 3", (Vector2){x, y}, 48, 2, WHITE);
                break;
            }
            }
        }
        renderer.drawTable(600, 430);
        for (int i = 0; i < 13; i++)
        {
            float x = 240.0f + (i * 60.0f);
            float y = 800.0f - 100;

            renderer.drawCard(i, x, y);
        }
        renderer.drawClock(40.0f, 60.0f, 140, 700, 35);
        renderer.drawMute(1100.0f, 40.0f);
        Vector2 size = MeasureTextEx(font, "Round 1", 48, 2);
        float x = 150.0 - size.x / 2;
        float y = 50.0 - size.y / 210;
        DrawTextEx(boldFont, "Round 1", (Vector2){x, y}, 48, 2, goldColor);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}