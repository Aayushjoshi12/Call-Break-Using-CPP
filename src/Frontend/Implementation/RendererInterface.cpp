#include "../headerfiles/RendererInterface.h"
#include <iostream>
#include <string>

Renderer::Renderer()
{
    cardBack = LoadTexture(IMG_CARD_BACK);
    table = LoadTexture(IMG_TABLE);
    background = LoadTexture(IMG_BACKGROUND);
    font = LoadFontEx(FONT_CINZEL, 96, 0, 0);
    boldFont = LoadFontEx(FONT_CINZEL, 96, 0, 0);
    goldColor = {255, 215, 0, 255};
    mute = LoadTexture(IMG_MUTE);

    for (int card = 0; card < 13; card++)
    {
        string path = "../Assets/Image files/cards/" + to_string(card + 2) + "_of_clubs.png";
        cards[card] = LoadTexture(path.c_str());
    }
}

void Renderer::drawCardBack(float positionofcardsX, float positionofcardsY)
{
    float scale = 70.0f / cardBack.width;
    DrawTextureEx(cardBack, {positionofcardsX - 35.0f, positionofcardsY - (cardBack.height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawTable(int TablePositionX, int TablePositionY)
{
    float scale = 800.0f / table.width;
    DrawTextureEx(table, {TablePositionX - 400.0f, TablePositionY - (table.height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawCard(int index, int Postionx, int Postiony)
{
    float scale = 50.0f / cards[index].width;
    DrawTextureEx(cards[index], {Postionx - 25.0f, Postiony - (cards[index].height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawBackground()
{
    DrawTexturePro(
        background,
        {0, 0, (float)background.width, (float)background.height},
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {0, 0}, 0.0f, WHITE);
}

void Renderer::drawMute(float Mutex, float Mutey)
{
    float scale = 30.0f / mute.width;
    DrawTextureEx(mute, {Mutex - 15.0f, Mutey - 15.0f}, 0.0f, scale, WHITE);
}

void Renderer::drawClock(float currentTime, float totalTime, int Circlex, int Circley, int radius)
{
    float fraction = currentTime / totalTime;
    float startAngle = -90.0f;
    float endAngle = -90.0f + (360.0f * fraction);

    DrawCircle(Circlex, Circley, radius, DARKGRAY);
    DrawCircleSector(
        {(float)Circlex, (float)Circley},
        radius, startAngle, endAngle, 36,
        fraction > 0.3f ? GREEN : RED);
    DrawCircleLines(Circlex, Circley, radius, WHITE);

    string text = to_string((int)currentTime);
    DrawText(text.c_str(), Circlex - 10, Circley - 10, 20, WHITE);
}
void Renderer::drawPlayedCards(vector<Move>& moves, int x, int y)
{
    // Center positions for each player's played card, relative to table center (x, y)
    // Player 0 = bottom, 1 = top, 2 = left, 3 = right
    float cx = (float)x, cy = (float)y;
    float offsets[4][2] = {
        { cx - 35,  cy + 80  },   // player 0 (You) - bottom center
        { cx - 35,  cy - 130 },   // player 1 (Bot1) - top center
        { cx - 150, cy - 25  },   // player 2 (Bot2) - left center
        { cx + 80,  cy - 25  }    // player 3 (Bot3) - right center
    };

    for (int i = 0; i < (int)moves.size(); i++)
    {
        int pid = moves[i].player_id;
        int slot = pid - 1;  // player_id is 1-based
        Card& c = moves[i].cardPlayed;
        float scale = 70.0f / c.texture.width;
        Vector2 pos = { offsets[slot][0], offsets[slot][1] };
        DrawTextureEx(c.texture, pos, 0.0f, scale, WHITE);
    }
}

void Renderer::drawWholeInterface(Card *hand, int count, Rectangle *rects,vector<Move> moves,float time,int round)

{
    string roundLabel = "Round " + std::to_string(round);
    drawBackground();

    // Bot 1 — top center
    drawCardBack(600.0, 100.0);
    Vector2 size1 = MeasureTextEx(font, "Bot 1", 48, 2);
    DrawTextEx(font, "Bot 1", {600.0f - size1.x / 2, 100.0f + 108.72f - size1.y - 10}, 48, 2, WHITE);

    // Bot 2 — left
    drawCardBack(200.0, 400.0);
    Vector2 size2 = MeasureTextEx(font, "Bot 2", 48, 2);
    DrawTextEx(font, "Bot 2", {200.0f - size2.x / 2, 400.0f + 108.72f - size2.y - 10}, 48, 2, WHITE);

    // Bot 3 — right
    drawCardBack(1000.0, 400.0);
    Vector2 size3 = MeasureTextEx(font, "Bot 3", 48, 2);
    DrawTextEx(font, "Bot 3", {1000.0f - size3.x / 2, 400.0f + 108.72f - size3.y - 10}, 48, 2, WHITE);

    drawTable(600, 430);

    // Player hand
    for (int i = 0; i < count; i++)
    {
        float x = 240.0f + (i * 60.0f);
        float y = 800.0f - 100;
        float scale = 50.0f / hand[i].texture.width;
        float w = hand[i].texture.width * scale;
        float h = hand[i].texture.height * scale;

        DrawTextureEx(hand[i].texture,
                      {x - 25.0f, y - h / 2.0f},
                      0.0f, scale, WHITE);

        // store the same on-screen rect for click/collision detection
        rects[i] = {x - 25.0f, y - h / 2.0f, w, h};
    }

    drawClock(time, 60.0f, 140, 700, 35);
    drawPlayedCards(moves, 600, 400);  

    // Round label
    Vector2 sizeR = MeasureTextEx(boldFont, roundLabel.c_str(), 48, 2);
    DrawTextEx(boldFont, roundLabel.c_str(), {150.0f - sizeR.x / 2, 50.0f - sizeR.y / 210.0f}, 48, 2, goldColor);

    // NOTE: mute/sound button is drawn by musicHandler.Draw() in main — not here
}
void Renderer::drawWholeInterface(Card *hand, int count,int round)

{
    string roundLabel = "Round " + std::to_string(round);
    drawBackground();

    // Bot 1 — top center
    drawCardBack(600.0, 100.0);
    Vector2 size1 = MeasureTextEx(font, "Bot 1", 48, 2);
    DrawTextEx(font, "Bot 1", {600.0f - size1.x / 2, 100.0f + 108.72f - size1.y - 10}, 48, 2, WHITE);

    // Bot 2 — left
    drawCardBack(200.0, 400.0);
    Vector2 size2 = MeasureTextEx(font, "Bot 2", 48, 2);
    DrawTextEx(font, "Bot 2", {200.0f - size2.x / 2, 400.0f + 108.72f - size2.y - 10}, 48, 2, WHITE);

    // Bot 3 — right
    drawCardBack(1000.0, 400.0);
    Vector2 size3 = MeasureTextEx(font, "Bot 3", 48, 2);
    DrawTextEx(font, "Bot 3", {1000.0f - size3.x / 2, 400.0f + 108.72f - size3.y - 10}, 48, 2, WHITE);

    drawTable(600, 430);

    // Player hand
    for (int i = 0; i < count; i++)
    {
        float x = 240.0f + (i * 60.0f);
        float y = 800.0f - 100;
        float scale = 50.0f / hand[i].texture.width;
        DrawTextureEx(hand[i].texture,
                      {x - 25.0f, y - (hand[i].texture.height * scale) / 2.0f},
                      0.0f, scale, WHITE);
    }

    

    // Round label
    Vector2 sizeR = MeasureTextEx(boldFont, roundLabel.c_str(), 48, 2);
    DrawTextEx(boldFont, roundLabel.c_str(), {150.0f - sizeR.x / 2, 50.0f - sizeR.y / 210.0f}, 48, 2, goldColor);

    // NOTE: mute/sound button is drawn by musicHandler.Draw() in main — not here
}

Renderer::~Renderer()
{
    UnloadTexture(cardBack);
    UnloadTexture(table);
    UnloadTexture(background);
    UnloadTexture(mute);
    for (int i = 0; i < 13; i++)
    {
        UnloadTexture(cards[i]);
    }
    UnloadFont(font);
    UnloadFont(boldFont);
}