#pragma once
#include "raylib.h"
#include <string>
using namespace std;

class Home_UI
{
public:
    Font      Cinzel_load;
    Texture2D Background_Img_load;
    Texture2D img_machine_load;
    Texture2D img_human_load;

    Home_UI();
    ~Home_UI();

    void Background();
    void img_machines(Rectangle Img_bot_machine_position);
    void img_human(Rectangle Img_bot_machine_position);
    void border_draw(Rectangle Border);
    void text_CallBreak();
    void Homedesign_draw();
    string isclicked();
};