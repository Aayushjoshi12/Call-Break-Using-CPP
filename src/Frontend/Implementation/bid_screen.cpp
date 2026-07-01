#include "../headerfiles/Bid_Screen.h"
#include"../text.h"
BidScreen::BidScreen()
{

    selectedBid = 0;
    cinzel = LoadFontEx(FONT_CINZEL, 96, 0, 0);
    confirmed = false;

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
    float panelH = 500;

    float panelX = (GetScreenWidth() - panelW) / 2;
    float panelY = (GetScreenHeight() - panelH) / 2;

    // Dark overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

    // Shadow
    DrawRectangleRounded({panelX + 8, panelY + 8, panelW, panelH}, 0.08f, 20, Fade(BLACK, 0.7f));

    // Main panel
    DrawRectangleRounded({panelX, panelY, panelW, panelH}, 0.08f, 20, (Color){20, 20, 20, 240});

    // Border
    DrawRectangleRoundedLinesEx({panelX, panelY, panelW, panelH}, 0.08f, 20, 3, GOLD);

    // Title
    DrawTextEx(cinzel, "CHOOSE YOUR BID", {panelX + 120, panelY + 25}, 38, 2, GOLD);

    // Top divider
    DrawLineEx({panelX + 40, panelY + 78}, {panelX + panelW - 40, panelY + 78}, 2, Fade(GOLD, 0.5f));

    // Bid circles (smaller radius + tighter spacing)
    float circleRadius = 42;
    for (int i = 0; i < 8; i++)
    {
        int row = i / 4;
        int col = i % 4;

        float centerX = panelX + 95 + col * 140;
        float centerY = panelY + 150 + row * 120;   // tighter row spacing

        bool selected = (selectedBid == i + 1);

        if (selected)
        {
            DrawCircle(centerX, centerY, circleRadius + 13, Fade(GOLD, 0.20f));
            DrawCircle(centerX, centerY, circleRadius + 6, Fade(GOLD, 0.35f));
        }

        Color chipColor = selected ? GOLD : (Color){210,210,210,255};

        DrawCircle(centerX, centerY, circleRadius, chipColor);
        DrawCircleLines(centerX, centerY, circleRadius, selected ? WHITE : DARKGRAY);
        DrawCircleLines(centerX, centerY, circleRadius - 7, selected ? WHITE : GRAY);

        const char* numLabel = TextFormat("%d", i + 1);
        Vector2 numSize = MeasureTextEx(cinzel, numLabel, 34, 2);
        DrawTextEx(
            cinzel,
            numLabel,
            {centerX - numSize.x / 2, centerY - numSize.y / 2},
            34,
            2,
            selected ? BLACK : DARKGRAY);
    }

    // Bottom divider
    float bottomLineY = panelY + panelH - 130;

    DrawLineEx({panelX + 40, bottomLineY}, {panelX + panelW - 40, bottomLineY}, 2, Fade(GOLD, 0.5f));

    // Selected bid text
    const char* bidLabel = TextFormat("SELECTED BID : %d", selectedBid);
    Vector2 bidTextSize = MeasureTextEx(cinzel, bidLabel, 26, 2);
    DrawTextEx(
        cinzel,
        bidLabel,
        {panelX + (panelW - bidTextSize.x) / 2, bottomLineY + 18},
        26,
        2,
        GOLD);

    // ---- CONFIRM BUTTON ----
    float btnW = 200;
    float btnH = 48;
    float btnX = panelX + (panelW - btnW) / 2;
    float btnY = panelY + panelH - 70;

    Rectangle confirmBtn = {btnX, btnY, btnW, btnH};

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, confirmBtn);
    bool enabled = (selectedBid > 0);

    Color btnFill =
        !enabled ? (Color){60,60,60,255}
        : hovered ? GOLD
        : (Color){180,140,30,255};

    Color btnText = !enabled ? GRAY : (hovered ? BLACK : WHITE);

    DrawRectangleRounded({btnX + 3, btnY + 3, btnW, btnH}, 0.3f, 12, Fade(BLACK, 0.5f));
    DrawRectangleRounded(confirmBtn, 0.3f, 12, btnFill);
    DrawRectangleRoundedLinesEx(confirmBtn, 0.3f, 12, 2, enabled ? WHITE : DARKGRAY);

    const char* label = "CONFIRM";
    Vector2 textSize = MeasureTextEx(cinzel, label, 24, 2);
    DrawTextEx(
        cinzel,
        label,
        {btnX + (btnW - textSize.x) / 2, btnY + (btnH - textSize.y) / 2},
        24,
        2,
        btnText);

    if (enabled && hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        confirmed = true;
    }
}

    int BidScreen::GetSelectedBid()
    {
        return selectedBid;
    }