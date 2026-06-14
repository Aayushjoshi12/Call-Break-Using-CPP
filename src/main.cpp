#include <raylib.h>
#include <iostream>
#include "Homedesign.h"
#include "human.h"

using namespace std;
float screenwidth = 1200, screenheight = 800;

Home_UI load_unload;

int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();
    load_unload.load();
    callall();
    PlayMusicStream(load_unload.mymusic);
    SetTargetFPS(60);
    // Rectangle button1 = {100, 200, 300, 150};
    // Rectangle button2 = {850, 200, 300, 150};
    while (WindowShouldClose() == false)
    {

        BeginDrawing();

        ClearBackground(RED);
        // Home design

        switch (currentstate)
        {
        case HomeScreen:
            load_unload.Homedesign_draw();
            break;

        case PlayBot:
            
            break;

        case PlayHuman:
            callfordrawing();
            break;

        default:
            break;
        }

        EndDrawing();
    }
    load_unload.unload();
    unloadall();
    CloseAudioDevice();
    CloseWindow();
}