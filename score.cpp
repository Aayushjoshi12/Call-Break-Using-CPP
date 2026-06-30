#include "ResultScreen.h"

ResultScreen::ResultScreen()
{
    myFont = LoadFont("Cinzel-Bold.ttf");

    for (int round = 0; round < 5; r++)
        for (int player = 0; player < 4; player++)
            scores[round][player] = 0;
}

ResultScreen::~ResultScreen()
{
    UnloadFont(myFont);
}

void ResultScreen::SetScore(int round, int player, int score)
{
    scores[round][player] = score;
}

void ResultScreen::Draw()
{
    int total[4] = {0}, winner = 0;
    int x[4] = {400, 620, 840, 1060};

    for (int player = 0; player < 4; p++)
        for (int round = 0; round < 5; r++)
            total[player] += scores[round][player];

    for (int player = 1; player< 4; player++)
        if (total[player] > total[winner])
            winner = player;

    DrawTextEx(myFont, "FINAL RESULT", {520, 40}, 40, 2, GOLD);

    for (int player = 0; player < 4; player++)
        DrawTextEx(myFont, TextFormat("PLAYER %d", p + 1), {(float)(x[p] - 40), 120}, 25, 2, WHITE);

    for (int round = 0, y = 220; round < 5; round++, y += 90)
    {
        DrawTextEx(myFont, TextFormat("ROUND %d", round + 1), {100, (float)y}, 28, 2, WHITE);
        for (int player = 0; player< 4; player++)
            if (scores[round][player])
                DrawTextEx(myFont, TextFormat("%d", scores[round][player]), {(float)x[player], (float)y}, 28, 2, YELLOW);
    }

    DrawTextEx(myFont, "TOTAL", {100, 680}, 32, 2, WHITE);

    for (int player = 0; player < 4; player++)
        if (total[player])
            DrawTextEx(myFont, TextFormat("%d", total[player]), {(float)x[player], 680}, 32, 2, GREEN);

    if (total[winner])
        DrawTextEx(myFont, TextFormat("CONGRATULATIONS PLAYER %d ON WINNING", winner + 1), {250, 720}, 28, 2, WHITE);
}
