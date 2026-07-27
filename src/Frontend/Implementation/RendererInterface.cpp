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
    if (totalTime <= 0.0f)
        return;

    float fraction = currentTime / totalTime;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

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
    // Player 0 = bottom, 1 = left, 2 = top, 3 = right (clockwise order)
    float cx = (float)x, cy = (float)y;
    float offsets[4][2] = {
        { cx - 35,  cy + 80  },   // player 0 (You) - bottom center
        { cx - 150, cy - 25  },   // player 1 (Bot 1) - left center
        { cx - 35,  cy - 130 },   // player 2 (Bot 2) - top center
        { cx + 80,  cy - 25  }    // player 3 (Bot 3) - right center
    };

    for (int i = 0; i < (int)moves.size(); i++)
    {
        int pid = moves[i].player_id;
        int slot = pid - 1;  // player_id is 1-based
        if (slot < 0 || slot >= 4)
            continue;

        Card& c = moves[i].cardPlayed;
        if (c.index < 1 || c.index > 52 || cardTextures[c.index - 1].width <= 0)
            continue;

        float scale = 70.0f / cardTextures[c.index - 1].width;
        Vector2 pos = { offsets[slot][0], offsets[slot][1] };
        DrawTextureEx(cardTextures[c.index - 1], pos, 0.0f, scale, WHITE);
    }
}

void Renderer::drawWholeInterface(Card *hand, int count, Rectangle *rects,vector<Move> moves,float time,int round)
{
    std::string labels[4] = {"You", "Bot 1", "Bot 2", "Bot 3"};
    drawWholeInterface(hand, count, rects, moves, time, round, labels, -1, -1);
}

void Renderer::drawWholeInterface(Card *hand, int count, Rectangle *rects,vector<Move> moves,float time,int round, const std::string (&labels)[4])
{
    drawWholeInterface(hand, count, rects, moves, time, round, labels, -1, -1);
}

void Renderer::drawWholeInterface(Card *hand, int count, Rectangle *rects,vector<Move> moves,float time,int round, const std::string (&labels)[4], int currentTurnPlayerId, int localPlayerId)
{
    int zeroBids[4] = {0, 0, 0, 0};
    int zeroTricks[4] = {0, 0, 0, 0};
    drawWholeInterface(hand, count, rects, moves, time, round, labels, currentTurnPlayerId, localPlayerId, zeroBids, zeroTricks);
}

void Renderer::drawWholeInterface(Card *hand, int count, Rectangle *rects,vector<Move> moves,float time,int round, const std::string (&labels)[4], int currentTurnPlayerId, int localPlayerId, const int (&bids)[4], const int (&tricksWon)[4])
{
    string roundLabel = "Round " + std::to_string(round);
    drawBackground();

    auto drawSeatLabel = [&](int slot, float x, float y, const std::string &name)
    {
        bool isActiveSeat = currentTurnPlayerId > 0 && (currentTurnPlayerId - 1) == slot;
        std::string label = name;
        if (isActiveSeat)
            label = (localPlayerId > 0 && currentTurnPlayerId == localPlayerId) ? "Your Turn" : "Turn";
        Color color = isActiveSeat ? GOLD : WHITE;
        Vector2 size = MeasureTextEx(font, label.c_str(), 36, 2);
        if(label=="Your Turn")
        {
            DrawTextEx(font, label.c_str(), {600-size.x/2, 600}, 36, 2, color);
        }
        else
        {
            DrawTextEx(font, label.c_str(), {x - size.x / 2, y}, 36, 2, color);
        }
        

        // Live bid / tricks-won readout for this seat
        std::string info = (bids[slot] > 0)
            ? TextFormat("Bid %d  |  Won %d", bids[slot], tricksWon[slot])
            : TextFormat("Won %d", tricksWon[slot]);
        Vector2 infoSize = MeasureTextEx(font, info.c_str(), 20, 1);
        DrawTextEx(font, info.c_str(), {x - infoSize.x / 2, y + 34}, 20, 1, goldColor);
    };

    // Left seat (Bot 1)
    drawCardBack(200.0, 400.0);
    drawSeatLabel(1, 200.0f, 400.0f + 108.72f - 10, labels[1]);

    // Top seat (Bot 2)
    drawCardBack(600.0, 100.0);
    drawSeatLabel(2, 600.0f, 55.0f + 108.72f - 10, labels[2]);

    // Right seat (Bot 3)
    drawCardBack(1000.0, 400.0);
    drawSeatLabel(3, 1000.0f, 400.0f + 108.72f - 10, labels[3]);

    // Bottom seat (local player)
    drawSeatLabel(0, 600.0f, 747.0f, labels[0]);

    drawTable(600, 430);

    // Player hand
    for (int i = 0; i < count; i++)
    {
        float x = 240.0f + (i * 60.0f);
        float y = 800.0f - 100;
        float scale = 50.0f / cardTextures[hand[i].index - 1].width;
        float w = cardTextures[hand[i].index - 1].width * scale;
        float h = cardTextures[hand[i].index - 1].height * scale;

        DrawTextureEx(cardTextures[hand[i].index - 1],
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
    std::string labels[4] = {"You", "Bot 1", "Bot 2", "Bot 3"};
    drawWholeInterface(hand, count, round, labels, -1, -1);
}

void Renderer::drawWholeInterface(Card *hand, int count,int round, const std::string (&labels)[4])
{
    drawWholeInterface(hand, count, round, labels, -1, -1);
}

void Renderer::drawWholeInterface(Card *hand, int count,int round, const std::string (&labels)[4], int currentTurnPlayerId, int localPlayerId)
{
    string roundLabel = "Round " + std::to_string(round);
    drawBackground();

    auto drawSeatLabel = [&](int slot, float x, float y, const std::string &name)
    {
        bool isActiveSeat = currentTurnPlayerId > 0 && (currentTurnPlayerId - 1) == slot;
        std::string label = name;
        if (isActiveSeat)
            label = (localPlayerId > 0 && currentTurnPlayerId == localPlayerId) ? "Your Turn" : "Turn";
        Color color = isActiveSeat ? GOLD : WHITE;
        Vector2 size = MeasureTextEx(font, label.c_str(), 36, 2);
          if(label=="Your Turn")
        {
            DrawTextEx(font, label.c_str(), {600-size.x/2, 600}, 36, 2, color);
        }
        else
        {
            DrawTextEx(font, label.c_str(), {x - size.x / 2, y}, 36, 2, color);
        }
        
    };

    // Left seat (Bot 1)
    drawCardBack(200.0, 400.0);
    drawSeatLabel(1, 200.0f, 400.0f + 108.72f - 10, labels[1]);

    // Top seat (Bot 2)
    drawCardBack(600.0, 100.0);
    drawSeatLabel(2, 600.0f, 55.0f + 108.72f - 10, labels[2]);

    // Right seat (Bot 3)
    drawCardBack(1000.0, 400.0);
    drawSeatLabel(3, 1000.0f, 400.0f + 108.72f - 10, labels[3]);

    // Bottom seat (local player)
    drawSeatLabel(0, 600.0f, 747.0f, labels[0]);

    drawTable(600, 430);

    // Player hand
    for (int i = 0; i < count; i++)
    {
        float x = 240.0f + (i * 60.0f);
        float y = 800.0f - 100;
        float scale = 50.0f / cardTextures[hand[i].index - 1].width;
        DrawTextureEx(cardTextures[hand[i].index - 1],
                      {x - 25.0f, y - (cardTextures[hand[i].index - 1].height * scale) / 2.0f},
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
    for (int i = 0; i < 52; i++)
    {
        UnloadTexture(cardTextures[i]);
    }
    UnloadFont(font);
    UnloadFont(boldFont);
}