#include<raylib.h>
#include "C:/Users/Aayush/Documents/Project Raylib/Call-Break-Using-CPP/Header folder/Homedesign.h"

void Home_UI::load(){
    clevis = LoadFontEx("Fonts/Cinzel-Bold.ttf",256,0,0);
    Background_Img = LoadTexture("Image files/HomeUI.png");
};
void Home_UI::unload(){
    UnloadTexture(Background_Img);
    UnloadFont(clevis);
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

