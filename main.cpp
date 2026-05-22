#include<raylib.h>
#include<iostream>

using namespace std;
float screenwidth = 900,screenheight = 900;


int main(){
    InitWindow(screenwidth,screenheight,"Cpp");
    Texture2D img = LoadTexture("Image files/BackGround.png");
    SetTargetFPS(100);
    while(WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(BLACK);
            DrawTexturePro(
            img,
            (Rectangle){ 0, 0, (float)img.width, (float)img.height },
            (Rectangle){ 0, 0, (float)screenwidth, (float)screenheight },
            (Vector2){ 0,0},
            0.0f,
            WHITE
        );
        EndDrawing();
    }
    UnloadTexture(img);
}