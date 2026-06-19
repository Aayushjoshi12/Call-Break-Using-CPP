*#include "ResultScreen.h"

ResultScreen::ResultScreen()
{
    myFont = LoadFont("Cinzel-Bold.ttf");

    for (int r = 0; r < 5; r++)
        for (int p = 0; p < 4; p++)
            scores[r][p] = 0;
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

    for (int p = 0; p < 4; p++)
        for (int r = 0; r < 5; r++)
            total[p] += scores[r][p];

    for (int p = 1; p < 4; p++)
        if (total[p] > total[winner])
            winner = p;

    DrawTextEx(myFont, "FINAL RESULT", {520, 40}, 40, 2, GOLD);

    for (int p = 0; p < 4; p++)
        DrawTextEx(myFont, TextFormat("PLAYER %d", p + 1), {(float)(x[p] - 40), 120}, 25, 2, WHITE);

    for (int r = 0, y = 220; r < 5; r++, y += 90)
    {
        DrawTextEx(myFont, TextFormat("ROUND %d", r + 1), {100, (float)y}, 28, 2, WHITE);
        for (int p = 0; p < 4; p++)
            if (scores[r][p])
                DrawTextEx(myFont, TextFormat("%d", scores[r][p]), {(float)x[p], (float)y}, 28, 2, YELLOW);
    }

    DrawTextEx(myFont, "TOTAL", {100, 680}, 32, 2, WHITE);

    for (int p = 0; p < 4; p++)
        if (total[p])
            DrawTextEx(myFont, TextFormat("%d", total[p]), {(float)x[p], 680}, 32, 2, GREEN);

    if (total[winner])
        DrawTextEx(myFont, TextFormat("CONGRATULATIONS PLAYER %d ON WINNING", winner + 1), {250, 720}, 28, 2, WHITE);
}
