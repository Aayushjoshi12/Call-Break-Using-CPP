#pragma once
#include "raylib.h"

class ResultScreen
{
public:
    ResultScreen();
    ~ResultScreen();
    void Draw(float scores[5][4]);

private:
    Font myFont;
    Texture2D background;
};