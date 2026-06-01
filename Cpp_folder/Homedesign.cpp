#include<raylib.h>
#include "C:/Users/Aayush/Documents/Project Raylib/Call-Break-Using-CPP/Header folder/Homedesign.h"

Color border_color_Home = {209, 194, 193,255};
Color Text_color_Home = {241,241,241,255};


//Loading and Unloading functions
void Home_UI::load(){ //For load image
    Cinzel_load = LoadFontEx("Fonts/Cinzel-Bold.ttf",256,0,0);
    Background_Img_load = LoadTexture("Image files/HomeUI.png");
    img_machine_load = LoadTexture("Image files/robot-bot-icon.png");
    img_human_load = LoadTexture("Image files/human.png");
    img_mute_button_load = LoadTexture("Image files/mute.png");
}
void Home_UI::unload(){ // For unloading image
    UnloadTexture(Background_Img_load);
    UnloadFont(Cinzel_load);
    UnloadTexture(img_machine_load);
    UnloadTexture(img_human_load);
    UnloadTexture(img_mute_button_load);
}


//General Homepage functions

void Home_UI::text_CallBreak(){
    DrawTextEx(
    Cinzel_load,
    "Call-Break Game",
    (Vector2){(float)((GetScreenWidth()/2.0)-240), 60},
    75,
    1,
    GOLD
);
      DrawTextEx(
    Cinzel_load,
    "PLAY WITH BOT",
    (Vector2){(float)160.0, (float)560.0},
    30,
    0.5f,
    Text_color_Home
);  
      DrawTextEx(
    Cinzel_load,
    "PLAY WITH HUMAN",
    (Vector2){(float)870.0, (float)560.0},
    30,
    0.5f,
    Text_color_Home
);
      DrawTextEx(
    Cinzel_load,
    "BY JUWADIS FOR JUWADIS",
    (Vector2){(float)GetScreenWidth()-400, (float)GetScreenHeight()-50},
    35,
    0.5f,
    GOLD
);
}

void Home_UI::border_draw(Rectangle Border){
    DrawRectangleRoundedLinesEx(Border,0.3f,20,2,border_color_Home);
}

void Home_UI::img_machines(Rectangle Img_bot_machine_position){
    DrawTexturePro(
    img_machine_load,
    (Rectangle){ 0, 0, (float)img_machine_load.width, (float)img_machine_load.height },
    Img_bot_machine_position,
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}
void Home_UI::img_human(Rectangle Img_bot_machine_position){
    DrawTexturePro(
    img_human_load,
    (Rectangle){ 0, 0, (float)img_human_load.width, (float)img_human_load.height },
    Img_bot_machine_position,
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}

void Home_UI:: Background(){ // Image Function

    DrawTexturePro(
    Background_Img_load,
    (Rectangle){ 0, 0, (float)Background_Img_load.width, (float)Background_Img_load.height },
    (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}

void Home_UI::Mute_button(Rectangle load){
    DrawTexturePro(
    img_mute_button_load,
    (Rectangle){ 0, 0, (float)img_mute_button_load.width, (float)img_mute_button_load.height },
    load,
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}
