#include <raylib.h>
#include <iostream>
#include "Frontend/Homedesign.h"
#include "Frontend/shuffling.h"
#include "Frontend/HandleMusic.h"
#include "Frontend/CardShuffler.h"
#include "Frontend/RendererInterface.h"
#include "Backend/entities.h"
#include "Frontend/bid_screen.h"

float screenwidth = 1200, screenheight = 800;
enum GameState
{
    Home,
    PlayBot,
    PlayHuman,
};
enum PlayBotState
{
    Shuffling,
    Dealing,
    Playing
};
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
    Renderer renderer;
    DealAnimation dealAnim;
    BidScreen bidScreen;

    float dealCardTimer = 0.0f;
    int cardsToShow = 0;

    bool dealtcards = false;
    bool dealStarted = false;

    cardShuffle.Init("../Assets/Image files/backhand.jpg");
    Player *players[4] = {
        new Player(true),  // human
        new Player(false), // bot 1
        new Player(false), // bot 2
        new Player(false)  // bot 3
    };

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
                currentstate = PlayBot;
            else if (HOME.isclicked() == "Human")
                currentstate = PlayHuman;
            break;
        }
        case PlayBot:
        {
            switch (currentplaybotstate)
            {
            case Shuffling:
            {
                cardShuffle.Update();
                if (!dealtcards)
                {
                    Deck deck;
                    deck.shuffle();
                    for (int i = 0; i < 52; i++)
                        players[i % 4]->receiveCard(deck.cardAt(i));
                    dealtcards = true;
                }
                if (cardShuffle.isDone()){
                    currentplaybotstate = Dealing;
                }
                break;
            }
            case Dealing:
            {
                if (!dealStarted)
                {
                    dealAnim.start(players[0]->hand, 13);
                    dealStarted = true;
                    cardsToShow = 0;
                }
                dealAnim.update(GetFrameTime());

                dealCardTimer += GetFrameTime();
                if (dealCardTimer >= 0.32f && cardsToShow < 13)
                { // 4 * 0.08 = every 4th card (player's turn)
                    cardsToShow++;
                    dealCardTimer = 0.0f;
                }

                if (dealAnim.isDone())
                    currentplaybotstate = Playing;
                break;
            }
            case Playing:
            { 
                bidScreen.Update();
                // Handle playing logic
                break;
            }
            } // ← closes inner switch (currentplaybotstate)
            break;
        }
        case PlayHuman:
        {
            std::cout << "Play Human mode selected" << std::endl;
            cardShuffle.Update();
            break;
        }
        } // ← closes outer switch (currentstate)

        // Draw
        BeginDrawing();

        switch (currentstate)
        {
        case Home:
        {
            ClearBackground(RED);
            HOME.Homedesign_draw();
            break;
        }
        case PlayBot:
        {
            ClearBackground({22, 82, 42, 255});
            switch (currentplaybotstate)
            {
            case Shuffling:
            {
                cardShuffle.Draw();
                break;
            }
            case Dealing:

            {
                renderer.drawWholeInterface(players[0]->hand, cardsToShow);
                dealAnim.draw(renderer);
                break;
            }
            case Playing:
            {
                renderer.drawWholeInterface(players[0]->hand, dealAnim.getDealtCount());
                bidScreen.Draw();

                break;
            }
            } // ← closes inner switch (currentplaybotstate)
            break;
        }
        case PlayHuman:
        {
            ClearBackground(RED);
            DrawText("Play Human mode selected", 400, 300, 20, BLACK);
            break;
        }
        } // ← closes outer switch (currentstate)

        musicHandler.Draw();
        EndDrawing();

    } // ← closes while loop

    for (int i = 0; i < 4; i++)
        delete players[i];
    cardShuffle.Unload();
    HOME.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}