#include "Renderer.h"

Renderer::Renderer()
{
    cardBack = LoadTexture("../Assets/Image files/backhand.jpeg");
    table = LoadTexture("../Assets/Image files/table.png");
    background = LoadTexture("../Assets/Image files/BackGround.png");
    for (int i = 0; i < 13; i++)
    {
        string path = "../Assets/Image files/cards/" + to_string(i+2) + "_of_clubs.png";
        cards[i] = LoadTexture(path.c_str());
    }
    mute=LoadTexture("../Assets/Image files/mute.png");
}

void Renderer::drawCardBack(float x, float y)
{
    float scale=70.0f/cardBack.width;

    DrawTextureEx(cardBack,{x-35.0f , y-(cardBack.height*scale)/2}, 0.0f, scale, WHITE);
}
void Renderer::drawTable(int x, int y)
{
      float scale=800.0f/table.width;
      DrawTextureEx(table,{x-400.0f , y-(table.height*scale)/2}, 0.0f, scale, WHITE);
}
void Renderer::drawCard(int index, int x, int y)
{
    float scale=50.0f/cards[index].width;
    DrawTextureEx(cards[index],{x-25.0f , y-(cards[index].height*scale)/2}, 0.0f, scale, WHITE);
}
void Renderer::drawBackground()
{
      DrawTexturePro(
        background,
        {0, 0, (float)background.width, (float)background.height},
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {0, 0}, 0.0f, WHITE
    );
                
}
void Renderer::drawMute(float x, float y){
    float scale=30.0f/mute.width;
    DrawTextureEx(mute,{x-15.0f , y-15.0f}, 0.0f, scale, WHITE);
}
void Renderer::drawClock(float currentTime, float totalTime, int x, int y, int radius) {
    float fraction = currentTime / totalTime;
    float startAngle = -90.0f;
    float endAngle = -90.0f + (360.0f * fraction);

    // background circle
    DrawCircle(x, y, radius, DARKGRAY);

    // pie slice
    DrawCircleSector(
        {(float)x, (float)y},
        radius,
        startAngle,
        endAngle,
        36,
        fraction > 0.3f ? GREEN : RED
    );

    // border
    DrawCircleLines(x, y, radius, WHITE);

    // countdown text in center
    string text = to_string((int)currentTime);
    DrawText(text.c_str(), x - 10, y - 10, 20, WHITE);
}
Renderer::~Renderer()
{
    UnloadTexture(cardBack);
    UnloadTexture(table);
    UnloadTexture(background);
    for (int i = 0; i < 13; i++)
    {
        UnloadTexture(cards[i]);
    }
    UnloadTexture(mute);
};