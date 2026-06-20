#include "../headerfiles/Bid_Screen.h"

BidScreen::BidScreen()
{

    selectedBid = 0;
    cinzel = LoadFontEx("../Assets/fonts/Cinzel_Font.ttf", 96, 0, 0);

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
    float panelW = 600;
    float panelH = 500;   // Increased height

    float panelX = (GetScreenWidth() - panelW) / 2;
    float panelY = (GetScreenHeight() - panelH) / 2;


    // Dark overlay
    DrawRectangle(
        0,
        0,
        GetScreenWidth(),
        GetScreenHeight(),
        Fade(BLACK, 0.55f));


    // Shadow
    DrawRectangleRounded(
        {panelX + 8, panelY + 8, panelW, panelH},
        0.08f,
        20,
        Fade(BLACK, 0.7f));


    // Main panel
    DrawRectangleRounded(
        {panelX, panelY, panelW, panelH},
        0.08f,
        20,
        (Color){20, 20, 20, 240});


    // Border
    DrawRectangleRoundedLinesEx(
        {panelX, panelY, panelW, panelH},
        0.08f,
        20,
        3,
        GOLD);


    // Title
    DrawTextEx(
        cinzel,
        "CHOOSE YOUR BID",
        {panelX + 120, panelY + 30},
        42,
        2,
        GOLD);


    // Top divider
    DrawLineEx(
        {panelX + 40, panelY + 90},
        {panelX + panelW - 40, panelY + 90},
        2,
        Fade(GOLD, 0.5f));


    // Bid circles
    for (int i = 0; i < 8; i++)
    {
        int row = i / 4;
        int col = i % 4;


        // New positions
        float centerX = panelX + 100 + col * 145;
        float centerY = panelY + 170 + row * 150;


        bool selected = (selectedBid == i + 1);


        // Glow
        if(selected)
        {
            DrawCircle(
                centerX,
                centerY,
                65,
                Fade(GOLD,0.20f));

            DrawCircle(
                centerX,
                centerY,
                57,
                Fade(GOLD,0.35f));
        }


        Color chipColor =
            selected
            ? GOLD
            : (Color){210,210,210,255};


        // Main chip
        DrawCircle(
            centerX,
            centerY,
            50,
            chipColor);


        // Outer border
        DrawCircleLines(
            centerX,
            centerY,
            50,
            selected ? WHITE : DARKGRAY);


        // Inner border
        DrawCircleLines(
            centerX,
            centerY,
            42,
            selected ? WHITE : GRAY);


        // Number
        DrawTextEx(
            cinzel,
            TextFormat("%d",i+1),
            {centerX - 12, centerY - 20},
            40,
            2,
            selected ? BLACK : DARKGRAY);
    }



    // Bottom divider
    float bottomLineY = panelY + panelH - 100;


    DrawLineEx(
        {panelX + 40,bottomLineY},
        {panelX + panelW - 40,bottomLineY},
        2,
        Fade(GOLD,0.5f));



    // Selected bid text
    DrawTextEx(
        cinzel,
        TextFormat("SELECTED BID : %d",selectedBid),
        {panelX + 170,bottomLineY + 25},
        32,
        2,
        GOLD);
}

    int BidScreen::GetSelectedBid()
    {
        return selectedBid;
    }