#pragma once
#include <iostream>
#include <raylib.h>
#include <string>
using namespace std;
class Renderer
{
private:
    Texture2D cardBack;
    Texture2D table;
    Texture2D cards[52];
    Texture2D background;
    Font font;
    Font boldFont;
    Color goldColor;
    Texture2D mute;

public:
    Renderer();
    ~Renderer();
    void drawCardBack(float x, float y);
    void drawTable(int x, int y);
    void drawCard(int index, int x, int y);
    void drawClock(float currentTime, float totalTime, int x, int y, int radius);
    void drawBackground();
    void drawMute(float x, float y);
    void drawWholeInterface();
};
