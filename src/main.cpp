#include <raylib.h>
#include <iostream>
#include "Frontend/Homedesign.h"
#include "Frontend/shuffling.h"
#include "Frontend/HandleMusic.h"

float screenwidth = 1200, screenheight = 800;
enum GameState { Home, PlayBot, PlayHuman, };
enum PlayBotState {Shuffling, Dealing, Playing}; 
GameState currentstate = Home;
PlayBotState currentplaybotstate = Shuffling;

Home_UI HOME;
CardShuffle cardShuffle;


int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();
    HOME.load();
    HandleMusic musicHandler;
    cardShuffle.Init("../Assets/Image files/backhand.jpg");
   
    SetTargetFPS(100);

    while (WindowShouldClose() == false)
    {
        // Update
        musicHandler.Update();

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
                switch(currentplaybotstate)
                {
                    case Shuffling:
                    {
                        cardShuffle.Update();
                        break;
                    }
                    case Dealing:
                    {
                        // Handle dealing logic
                        break;
                    }
                    case Playing:
                    {
                        // Handle playing logic
                        break;
                    }
                }
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
        
        

        switch (currentstate)
        {   ClearBackground(RED);
            case Home:
            {
                HOME.Homedesign_draw();
                break;
            }
            case PlayBot:
            { 
              ClearBackground({ 22, 82, 42, 255 });
             switch(currentplaybotstate){
                    case Shuffling:
                    {
                        cardShuffle.Draw();
                        break;
                    }
                    case Dealing:
                    {
                        // Handle dealing drawing
                        break;
                    }
                    case Playing:
                    {
                        // Handle playing drawing
                        break;
                    }
                }
                break;
             }
                
            
            case PlayHuman:
            {
                ClearBackground(RED);
                DrawText("Play Human mode selected", 400, 300, 20, BLACK);
                break;
            }
        }
        musicHandler.Draw();

        EndDrawing();
    }

    cardShuffle.Unload();
    HOME.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}