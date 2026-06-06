#pragma once
#include "raylib.h"
#include <string>
using namespace std;
class Home_UI
{
public:
    void Background();

    // Loading variable

    Font Cinzel_load;
    Texture2D Background_Img_load;
    Texture2D img_machine_load;
    Texture2D img_human_load;
    Texture2D img_mute_button_load;

    // Function loading and unloading declaration
    void load();
    void unload();

    // Function Declaration for Home page
    void img_machines(Rectangle Img_bot_machine_position);
    void img_human(Rectangle Img_bot_machine_position);
    void border_draw(Rectangle Border);
    void Mute_button(Rectangle load);
    void text_CallBreak();
};
