#include <raylib.h>
#include <iostream>
#include "Homedesign.h"
#include "CardShuffle.h"
#include "human.h"
using namespace std;

float screenwidth = 1200, screenheight = 800;
bool shuffled = false;
Home_UI load_unload;

int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();

    Renderer renderer;

    load_unload.load();
    callall();
    PlayMusicStream(load_unload.mymusic);
    SetTargetFPS(60);

    while (WindowShouldClose() == false)
    {
        float dt = GetFrameTime();

        // Update animation
        if (currentstate == PlayBot)
            dealAnim.update(dt);

        BeginDrawing();
        ClearBackground(RED);

        switch (currentstate)
        {
        case HomeScreen:
            load_unload.Homedesign_draw();
            break;

        case PlayBot:
            if (!shuffled)
            {
                callforshuffling();
                shuffled = true;
            }
            renderer.drawWholeInterface();  // draws bg, table, bots, labels
            dealAnim.draw(renderer);        // draws flying cards on top
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