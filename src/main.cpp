#include <raylib.h>
#include <iostream>
#include "Frontend/Homedesign.h"
#include "Frontend/shuffling.h"

float screenwidth = 1200, screenheight = 800;
enum GameState { Home, PlayBot, PlayHuman };
GameState currentstate = Home;

Home_UI HOME;
CardShuffle cardShuffle;

int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();
    HOME.load();
    cardShuffle.Init("../Assets/Image files/backhand.jpg");
    PlayMusicStream(HOME.mymusic);
    SetTargetFPS(100);

    while (WindowShouldClose() == false)
    {
        // Update
        UpdateMusicStream(HOME.mymusic);

        switch (currentstate)
        {
            case Home:
            {
                if (HOME.isclicked() == "Bot")
                {
                    currentstate = PlayBot;
                }
                else if (HOME.isclicked() == "Human")
                {
                    currentstate = PlayHuman;
                }
                break;
            }
            case PlayBot:
            {
                std::cout << "Play Bot mode selected" << std::endl;
                cardShuffle.Update();
                break;
            }
            case PlayHuman:
            {
                std::cout << "Play Human mode selected" << std::endl;
                cardShuffle.Update();
                break;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RED);

        switch (currentstate)
        {
            case Home:
            {
                HOME.Homedesign_draw();
                break;
            }
            case PlayBot:
            { ClearBackground({ 22, 82, 42, 255 });
                cardShuffle.Draw();
                break;
            }
            case PlayHuman:
            {
                DrawText("Play Human mode selected", 400, 300, 20, BLACK);
                break;
            }
        }

        EndDrawing();
    }

    cardShuffle.Unload();
    HOME.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}