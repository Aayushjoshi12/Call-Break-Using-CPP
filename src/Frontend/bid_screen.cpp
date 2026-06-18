#include "bid_screen.h"

BidScreen::BidScreen()
{

    selectedBid = 0;
    cinzel = LoadFont("../Assets/fonts/Cinzel_Font.ttf");

    float panelX = (GetScreenWidth() - 500) / 2;
    float panelY = (GetScreenHeight() - 500) / 2;

    // Row 1 — inside panel
    bidButtons[0] = {panelX + 20, panelY + 150, 100, 100};
    bidButtons[1] = {panelX + 140, panelY + 150, 100, 100};
    bidButtons[2] = {panelX + 260, panelY + 150, 100, 100};
    bidButtons[3] = {panelX + 380, panelY + 150, 100, 100};

    // Row 2 — inside panel
    bidButtons[4] = {panelX + 20, panelY + 300, 100, 100};
    bidButtons[5] = {panelX + 140, panelY + 300, 100, 100};
    bidButtons[6] = {panelX + 260, panelY + 300, 100, 100};
    bidButtons[7] = {panelX + 380, panelY + 300, 100, 100};
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

void BidScreen::Draw()
{
    float panelX = (GetScreenWidth() - 500) / 2;
    float panelY = (GetScreenHeight() - 500) / 2;

    // Replace DrawTexturePro with this for the panel background
    DrawRectangle(panelX, panelY, 500, 500, (Color){0, 0, 0, 200});
    DrawRectangleLinesEx((Rectangle){panelX, panelY, 500, 500}, 3, GOLD);

    DrawTextEx(cinzel, "ENTER YOUR BID", (Vector2){panelX + 80, panelY + 30}, 45, 2, GOLD);

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

        DrawTextEx(
            cinzel,
            TextFormat("%d", buttonIndex + 1),
            (Vector2){bidButtons[buttonIndex].x + 35, bidButtons[buttonIndex].y + 25},
            40,
            2,  
            BLACK);
    }

    DrawTextEx(
        cinzel,
        TextFormat("Selected Bid : %d", selectedBid),
        (Vector2){panelX + 120, panelY + 450},
        35,
        2,
        GREEN);
    }

    int BidScreen::GetSelectedBid()
    {
        return selectedBid;
    }