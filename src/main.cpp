#include <raylib.h>
#include <iostream>
#include "Homedesign.h"

using namespace std;
float screenwidth = 1200, screenheight = 800;

Home_UI load_unload;


int main()
{
    
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();
    load_unload.load();
    PlayMusicStream(load_unload.mymusic);
    SetTargetFPS(100);
    // Rectangle button1 = {100, 200, 300, 150};
    // Rectangle button2 = {850, 200, 300, 150};
    while (WindowShouldClose() == false)
    {

        BeginDrawing();

        ClearBackground(RED);
        // Home design
        load_unload.run_HomeDesign_function();


        EndDrawing();
    }
    load_unload.unload();
    CloseAudioDevice();
    CloseWindow();
}