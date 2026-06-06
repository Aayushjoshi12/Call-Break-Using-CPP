
#include <iostream>
#include <raylib.h>
#include <string>
#include "human.hpp"
#include "mute.hpp"
#include "font.hpp"
using namespace std;
int main()
{
    InitWindow(1200, 800, "Call break");
    SetTargetFPS(60);
    // font
    Font font = LoadFontEx("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    Texture2D cardback = LoadTexture("C:\\Users\\rajes\\Downloads\\background (1) (1).png");
    man player1("C:\\Users\\rajes\\Downloads\\human.png", (Vector2){50, 100}, 0.8);
    man player2("C:\\Users\\rajes\\Downloads\\human.png", (Vector2){800, 100}, 0.8);
    man player3("C:\\Users\\rajes\\Downloads\\human.png", (Vector2){50, 400}, 0.8);
    man player4("C:\\Users\\rajes\\Downloads\\human.png", (Vector2){800, 400}, 0.8);
    mute mute1("C:\\Users\\rajes\\Downloads\\mute.png", (Vector2){1100, 20}, 0.0987);
    fonta font1("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);
    fonta font2("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);
    fonta font3("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);
    fonta font4("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);
    fonta font5("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);
    fonta font6("C:\\Users\\rajes\\Downloads\\Cinzel-Bold.ttf", 48);

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(cardback, 0, 0, WHITE);
        DrawTextEx(font, "Game lobby", (Vector2){400, 40}, font.baseSize * 1.5, 2, YELLOW);
        player1.draw();
        font1.drawf("Player 1", (Vector2){120, 100}, font.baseSize * 0.8, 2, WHITE);
        player1.drawBox({WHITE}, 3.0f);
        player2.draw();
        player2.drawBox({WHITE}, 3.0f);
        font2.drawf("Player 2", (Vector2){870, 100}, font.baseSize * 0.8, 2, WHITE);

        player3.draw({255, 255, 255, 120});
        player3.drawBox({GRAY}, 3.0f);
        font3.drawf("Player 3", (Vector2){120, 400}, font.baseSize * 0.8, 2, WHITE);
        player4.draw({255, 255, 255, 120});
        player4.drawBox({GRAY}, 3.0f); // we can use {255,255,255,120} instead of gray
        font4.drawf("Player 4", (Vector2){870, 400}, font.baseSize * 0.8, 2, WHITE);
        font5.drawf("2/4 have come", (Vector2){450, 650}, font.baseSize * 0.8, 2, WHITE);
        font6.drawf("Waiting for players...", (Vector2){300, 700}, font.baseSize * 1.2, 2, YELLOW);

        mute1.drawm();
        EndDrawing();
    }
    UnloadFont(font);
    CloseWindow();
    return 0;
}
