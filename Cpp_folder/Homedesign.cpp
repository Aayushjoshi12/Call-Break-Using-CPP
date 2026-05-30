#include<raylib.h>
#include "C:/Users/Aayush/Documents/Project Raylib/Call-Break-Using-CPP/Header folder/Homedesign.h"

void Home_UI::load(){ //For load image
    clevis = LoadFontEx("Fonts/Cinzel-Bold.ttf",256,0,0);
    Background_Img = LoadTexture("Image files/HomeUI.png");
    img_machine = LoadTexture("Image files/robot-bot-icon.png");
};
void Home_UI::unload(){ // For unloading image
    UnloadTexture(Background_Img);
    UnloadFont(clevis);
    UnloadTexture(img_machine);
}
// void Home_UI::button_draw(float x,float y){
//     DrawRectangle(x, y, 300, 150, RED);
// }
void Home_UI::text_CallBreak(){
    DrawTextEx(
    clevis,
    "Call-Break Game",
    (Vector2){(float)((GetScreenWidth()/2.0)-240), 60},
    75,
    1,
    GOLD
);
}

void Home_UI::border_draw(Rectangle Border){
    DrawRectangleLinesEx(Border,2,BROWN);
}

void Home_UI::img_machines(Rectangle Img_bot_machine_position){

    DrawTexturePro(
    img_machine,
    (Rectangle){ 0, 0, (float)img_machine.width, (float)img_machine.height },
    Img_bot_machine_position,
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}

void Home_UI:: Background(){ // Image Function

    DrawTexturePro(
    Background_Img,
    (Rectangle){ 0, 0, (float)Background_Img.width, (float)Background_Img.height },
    (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
    (Vector2){ 0,0},
    0.0f,
    WHITE
        );
}

