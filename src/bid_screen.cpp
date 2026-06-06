#include <raylib.h>

int selectedBid = 0;

void DrawBidScreen(Texture2D bg)
{
    Vector2 mouse = GetMousePosition();

    Rectangle buttons[8] =
        {
            {150, 250, 100, 100},
            {330, 250, 100, 100},
            {510, 250, 100, 100},
            {690, 250, 100, 100},

            {150, 450, 100, 100},
            {330, 450, 100, 100},
            {510, 450, 100, 100},
            {690, 450, 100, 100}};

    for (int i = 0; i < 8; i++)
    {
        if (CheckCollisionPointRec(mouse, buttons[i]) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectedBid = i + 1;
        }
    }

    DrawTexturePro(
        bg,
        (Rectangle){0, 0, (float)bg.width, (float)bg.height},
        (Rectangle){0, 0, 900, 900},
        (Vector2){0, 0},
        0,
        WHITE);

    DrawText("ENTER YOUR BID", 250, 100, 45, GOLD);

    for (int i = 0; i < 8; i++)
    {
        Color btnColor = LIGHTGRAY;

        if (selectedBid == i + 1)
            btnColor = YELLOW;

        DrawCircle(
            buttons[i].x + 50,
            buttons[i].y + 50,
            50,
            btnColor);

        DrawText(
            TextFormat("%d", i + 1),
            buttons[i].x + 35,
            buttons[i].y + 25,
            40,
            BLACK);
    }

    DrawText(
        TextFormat("Selected Bid : %d", selectedBid),
        300,
        700,
        35,
        GREEN);
}