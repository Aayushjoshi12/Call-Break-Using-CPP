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
    Texture2D img_machine;
    Rectangle Img_bot_machine_position;
    void img_machines(Rectangle Img_bot_machine_position);
    void load();
    void border_draw(Rectangle Border);
    void unload();
    // void button_draw(float x,float y);
    void text_CallBreak();
};
