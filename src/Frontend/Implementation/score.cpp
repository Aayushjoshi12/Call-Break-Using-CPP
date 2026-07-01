#include "../headerfiles/score.h"
#include <iostream>

ResultScreen::ResultScreen()
{
    myFont = LoadFontEx("../Assets/fonts/Cinzel-Bold.ttf", 96, 0, 0);
    background = LoadTexture("../Assets/Image files/BackGround.png");
}

ResultScreen::~ResultScreen()
{
    UnloadFont(myFont);
    UnloadTexture(background);
}

void ResultScreen::Draw(float scores[5][4])
{
    // Draw background first so everything else renders on top of it
    DrawTexturePro(
        background,
        {0, 0, (float)background.width, (float)background.height},
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {0, 0}, 0.0f, WHITE);

    int total[4] = {0}, winner = 0;
    int x[4] = {400, 620, 840, 1060};

    for (int player = 0; player < 4; player++)
        for (int round = 0; round < 5; round++)
            total[player] += scores[round][player];

    for (int player = 1; player < 4; player++)
        if (total[player] > total[winner])
            winner = player;

    DrawTextEx(myFont, "FINAL RESULT", {520, 40}, 40, 2, GOLD);

    for (int player = 0; player < 4; player++)
        DrawTextEx(myFont, TextFormat("PLAYER %d", player + 1), {(float)(x[player] - 40), 120}, 25, 2, WHITE);

    for (int round = 0, y = 220; round < 5; round++, y += 90)
    {
        DrawTextEx(myFont, TextFormat("ROUND %d", round + 1), {100, (float)y}, 28, 2, WHITE);
        for (int player = 0; player < 4; player++)
        {
            int val = scores[round][player];
            Color c = (val < 0) ? RED : (val > 0) ? YELLOW : GRAY;
            DrawTextEx(myFont, TextFormat("%d", val), {(float)x[player], (float)y}, 28, 2, c);
        }
    }

    DrawTextEx(myFont, "TOTAL", {100, 680}, 32, 2, WHITE);

    for (int player = 0; player < 4; player++)
    {
        Color c = (total[player] < 0) ? RED : GREEN;
        DrawTextEx(myFont, TextFormat("%d", total[player]), {(float)x[player], 680}, 32, 2, c);
    }

    DrawTextEx(myFont, TextFormat("CONGRATULATIONS PLAYER %d ON WINNING", winner + 1), {300, 750}, 28, 2, WHITE);
}