#include "bid_screen.h"

BidScreen::BidScreen()
{
    selectedBid = 0;

    bidButtons[0] = {150, 250, 100, 100};
    bidButtons[1] = {330, 250, 100, 100};
    bidButtons[2] = {510, 250, 100, 100};
    bidButtons[3] = {690, 250, 100, 100};

    bidButtons[4] = {150, 450, 100, 100};
    bidButtons[5] = {330, 450, 100, 100};
    bidButtons[6] = {510, 450, 100, 100};
    bidButtons[7] = {690, 450, 100, 100};
}

void BidScreen::Update()
{
    Vector2 mousePosition = GetMousePosition();

    for (int buttonIndex = 0; buttonIndex < 8; buttonIndex++)
    {
        if (CheckCollisionPointRec(mousePosition, bidButtons[buttonIndex]) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectedBid = buttonIndex + 1;
        }
    }
}

void BidScreen::Draw(Texture2D backgroundImage)
{
    DrawTexturePro(
        backgroundImage,
        (Rectangle){0, 0, (float)backgroundImage.width, (float)backgroundImage.height},
        (Rectangle){0, 0, 900, 900},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    DrawText("ENTER YOUR BID", 250, 100, 45, GOLD);

    for (int buttonIndex = 0; buttonIndex < 8; buttonIndex++)
    {
        Color buttonColor = LIGHTGRAY;

        if (selectedBid == buttonIndex + 1)
        {
            buttonColor = YELLOW;
        }

        DrawCircle(
            bidButtons[buttonIndex].x + 50,
            bidButtons[buttonIndex].y + 50,
            50,
            buttonColor);

        DrawText(
            TextFormat("%d", buttonIndex + 1),
            bidButtons[buttonIndex].x + 35,
            bidButtons[buttonIndex].y + 25,
            40,
            BLACK);
    }

    DrawText(
        TextFormat("Selected Bid : %d", selectedBid),
        280,
        700,
        35,
        GREEN);
}

int BidScreen::GetSelectedBid()
{
    return selectedBid;
}