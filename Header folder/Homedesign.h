#pragma once
#include"raylib.h"
#include<string>
using namespace std;
class Home_UI{
public:
    void Background();
    float x=400,y=700;
    Font clevis;
    Texture2D Background_Img;
    // Font Cleviz = ;
    void load();
    void unload();
    // void button_draw(float x,float y);
    void text_CallBreak();

};
