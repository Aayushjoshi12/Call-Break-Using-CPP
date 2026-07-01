#include <raylib.h>
#include <iostream>
#include "Frontend/headerfiles/Homedesign.h"
#include "Frontend/headerfiles/Shuffling.h"
#include "Frontend/headerfiles/HandleMusic.h"
#include "Frontend/headerfiles/Dealing_Cards.h"
#include "Frontend/headerfiles/RendererInterface.h"
#include "Backend/headerfiles/entities.h"
#include "Frontend/headerfiles/Bid_Screen.h"
#include "Backend/headerfiles/GameManager.h"
#include "Frontend/headerfiles/score.h"

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
    Playing,
    displayscores
};
GameState currentstate = Home;
PlayBotState currentplaybotstate = Shuffling;

int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    InitAudioDevice();
    Home_UI HOME;
    HOME.load();
    CardShuffle cardShuffle;
    GameManager game;
    float scoredelay = 10.0f;
    HandleMusic musicHandler;
    Renderer renderer;
    ResultScreen score;
    DealAnimation dealAnim;
    BidScreen bidScreen;

    float dealCardTimer = 0.0f;
    int cardsToShow = 0;

    bool dealtcards = false;
    bool dealStarted = false;
    bool bidchosen = false;

    cardShuffle.Init("../Assets/Image files/backhand.jpg");
    Player *players[4] = {
        new Player(true),  // human
        new Player(false), // bot 1
        new Player(false), // bot 2
        new Player(false)  // bot 3
    };

    // ASSUMPTION: player_id is not assigned in the Player constructor.
    // If entities.h already sets it, remove this loop.
    for (int i = 0; i < 4; i++)
        players[i]->player_id = i + 1; // 1..4, human = 1

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
                    {
                        players[i % 4]->receiveCard(deck.cardAt(i));
                        players[i % 4]->organizeHand();
                    }

                    dealtcards = true;
                }
                if (cardShuffle.isDone())
                {
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
                if (!bidchosen)
                {
                    bidScreen.Update();
                    if (bidScreen.confirmed)
                    {   
                        players[0]->bid = bidScreen.GetSelectedBid();
                        for (int bot = 1; bot < 4; bot++)
                        {
                            players[bot]->bid = players[bot]->chooseBid();
                        }
                        bidchosen = true;

                        // Start the first trick of the hand now that bidding is done.
                        game.roundManager.startRound(1, players);
                        game.timeManager.reset();
                    }
                }

                if (game.needNewHand)
                {
                    game.needNewHand = false;
                    bidchosen = false;
                    dealtcards = false;
                    dealStarted = false;
                    cardsToShow = 0;         
                    dealCardTimer = 0.0f;
                    bidScreen.confirmed = false;
                    cardShuffle.Reset();

                    for (int i = 0; i < 4; i++)
                    {
                        players[i]->tricksWon = 0;
                        players[i]->bid = 0;
                        for (int j = 0; j < players[i]->handSize; j++)
                            players[i]->hand[j].unload();
                        players[i]->handSize = 0;
                       
                        
                    }

                    currentplaybotstate = displayscores;
                }

                game.updateGame(players);
                for(int i = 0; i < 4; i++)
                {
                    players[i]->organizeHand();
                }

                break;
            }
            case displayscores:
            {
                scoredelay -= GetFrameTime();
                if (scoredelay <= 0)
                {
                    currentplaybotstate = Shuffling;
                    scoredelay = 30.0f;
                }
                break;
            }
            } // closes switch (currentplaybotstate)
            break;
        } // closes case PlayBot
        case PlayHuman:
        {
            std::cout << "Play Human mode selected" << std::endl;
            cardShuffle.Update();
            break;
        }
        } // closes outer switch (currentstate)

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
                renderer.drawWholeInterface(players[0]->hand, cardsToShow, game.handsPlayed + 1);
                dealAnim.draw(renderer);
                break;
            }
            case Playing:
            {
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, players[0]->rects, game.roundManager.moves, game.timeManager.currentTime,game.handsPlayed + 1);
                if (!bidchosen)
                {
                    bidScreen.Draw();
                }

                break;
            }
            case displayscores:
            {
                ClearBackground(BLACK);
                score.Draw(game.scores);
                break;
            }
            } // closes inner switch (currentplaybotstate)
            break;
        }
        case PlayHuman:
        {
            ClearBackground(RED);
            DrawText("Play Human mode selected", 400, 300, 20, BLACK);
            break;
        }
        } // closes outer switch (currentstate)

        musicHandler.Draw();
        EndDrawing();

    } // closes while loop

    for (int i = 0; i < 4; i++)
        delete players[i];
    cardShuffle.Unload();
    HOME.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
