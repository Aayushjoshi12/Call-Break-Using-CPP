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
#include "Backend/headerfiles/RoundManager.h" // needed for the Move type used by humanMoves
#include "Frontend/headerfiles/score.h"
#include "Network/headerfiles/client.h"
#include "Frontend/headerfiles/lobby.h"

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

enum PlayHumanState
{
    Connecting,
    Lobby,
    humanShuffling,
    humanDealing,
    humanBidding, // NEW: every client picks their own bid here
    humanPlaying,
    WaitingForTurn,
    displayscoresHuman
};

GameState currentstate = Home;
PlayBotState currentplaybotstate = Shuffling;
PlayHumanState currentplayhumanstate = Connecting;

int main()
{
    InitWindow(screenwidth, screenheight, "Call Break");
    bool lobbyFullMessage = false;

    std::string homeStatusMessage = "";
    InitAudioDevice();
    Font font = LoadFontEx("../Assets/Fonts/Cinzel_Font.ttf", 96, 0, 0);

    Home_UI HOME;
    HOME.load();

    CardShuffle cardShuffle;
    GameManager game;
    GameLobby lobbyScreen;
    Client client;
    HandleMusic musicHandler;
    Renderer renderer;
    ResultScreen score;
    DealAnimation dealAnim;
    BidScreen bidScreen;

    float scoredelay = 10.0f;

    float dealCardTimer = 0.0f;
    int cardsToShow = 0;

    bool dealtcards = false;
    bool dealStarted = false;
    bool bidchosen = false;
    // NEW: the bid screen was disappearing almost instantly because
    // selectedBid persisted from the previous hand. This enforces a real
    // decision window: at least BID_SCREEN_MIN_TIME must pass before any
    // confirm is accepted (guards against stale state), and the player
    // gets up to BID_SCREEN_TIMEOUT seconds before a bid is auto-picked.
    float bidScreenTimer = 0.0f;
    const float BID_SCREEN_MIN_TIME = 0.5f;
    const float BID_SCREEN_TIMEOUT = 40.0f;

    // ── human-mode networking state ──
    bool cardsReceived = false;
    const float SHUFFLE_DELAY = 10.0f;
    float shuffleDelay = SHUFFLE_DELAY;

    // Tracks last known lobby count for drawing (queue is consumed in Update, not Draw)
    int lastPlayersConnected = 0;

    // state needed to actually play a networked hand
    bool myTurn = false;
    int pendingCardIndex = -1;
    int currentTurnPlayerId = -1;
    int localPlayerId = -1;
    std::string seatLabels[4] = {"You", "Player 2", "Player 3", "Player 4"};
    std::vector<Move> humanMoves;
    int humanHandsPlayed = 0;

    
    int seatBids[4] = {0, 0, 0, 0};
    int seatTricks[4] = {0, 0, 0, 0};

    float humanScoresGrid[5][4] = {};
    bool matchOverPendingHome = false;

    bool bidSubmitted = false;
    bool bidAutoSubmitted = false;
    bool biddingCompleteReceived = false;
    float bidAutoTimer = 0.0f;

    const float BID_AUTO_SUBMIT_DELAY = 180.0f;

 
    bool humanTrickEndPending = false;
    float humanTrickEndTimer = 0.0f;
    const float HUMAN_TRICK_END_DELAY = 1.2f;
    float humanScoreDelay = 10.0f;
    const float HUMAN_SCORE_DELAY = 10.0f;
    float humanDealingTimer = 0.0f;
    const float HUMAN_DEALING_FALLBACK_DELAY = 2.2f;

    cardShuffle.Init("../Assets/Image files/backhand.jpg");
    bool playersobjectsCreated = false;
    Player *players[4] = {nullptr, nullptr, nullptr, nullptr};

    SetTargetFPS(100);

    auto refreshCardRects = [&](Player *p)
    {
        if (!p)
            return;

        for (int i = 0; i < p->handSize; ++i)
        {
            float x = 240.0f + (i * 60.0f);
            float y = 800.0f - 100.0f;
            float scale = 50.0f / 100.0f;
            float w = 70.0f * scale;
            float h = 100.0f * scale;
            p->rects[i] = {x - 25.0f, y - h / 2.0f, w, h};
        }
    };

    while (WindowShouldClose() == false)
    {
        musicHandler.Update();

        switch (currentstate)
        {
        case Home:
        {
            if (HOME.isclicked() == "Bot")
                currentstate = PlayBot;
            else if (HOME.isclicked() == "Human")
            {
                bool connected = false;
                const char *candidates[] = {"127.0.0.1", "localhost", "192.168.1.67"};
                for (const char *host : candidates)
                {
                    if (client.init(host, 54000))
                    {
                        std::cout << "Client connected to server at " << host << std::endl;
                        connected = true;
                        break;
                    }
                    std::cout << "Client failed to connect to " << host << std::endl;
                }

                if (!connected)
                {
                    std::cerr << "Failed to initialize client. Exiting." << std::endl;
                    return -1;
                }
                currentstate = PlayHuman;
                
            }
            break;
        }

        case PlayBot:
        {
            if (!playersobjectsCreated)
            {
                players[0] = new Player(true);  // human
                players[1] = new Player(false); // bot 1
                players[2] = new Player(false); // bot 2
                players[3] = new Player(false); // bot 3
                for (int i = 0; i < 4; i++)
                    players[i]->player_id = i + 1;
                playersobjectsCreated = true;
            }

            switch (currentplaybotstate)
            {
            case Shuffling:
            {
                if (cardShuffle.IsSkipButtonPressed())
                    cardShuffle.Skip();

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
                {
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
                    bidScreenTimer += GetFrameTime();

                    bool realConfirm = bidScreen.confirmed && bidScreenTimer >= BID_SCREEN_MIN_TIME;
                    bool timedOut = bidScreenTimer >= BID_SCREEN_TIMEOUT;

                    if (realConfirm || timedOut)
                    {
                        int myBid = realConfirm ? bidScreen.GetSelectedBid() : players[0]->chooseBid();
                        if (myBid < 1)
                            myBid = players[0]->chooseBid();

                        players[0]->bid = myBid;
                        for (int bot = 1; bot < 4; bot++)
                        {
                            players[bot]->bid = players[bot]->chooseBid();
                        }
                        bidchosen = true;

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
                    bidScreen.Reset();
                    bidScreenTimer = 0.0f;
                    cardShuffle.Reset();

                    for (int i = 0; i < 4; i++)
                    {
                        players[i]->tricksWon = 0;
                        players[i]->bid = 0;
                        players[i]->handSize = 0;
                    }

                    currentplaybotstate = displayscores;
                }
                if (game.gameOver)
                {
                    currentstate = Home;
                    game.gameOver = false;
                    game.handsPlayed = 0;
                    game.roundsPlayed = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        players[i]->score = 0.0f;
                    }
                }

                game.updateGame(players, GetFrameTime());
                for (int i = 0; i < 4; i++)
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
                    scoredelay = 10.0f;
                }
                break;
            }
            } // end switch(currentplaybotstate)
            break;
        } // end case PlayBot

        case PlayHuman:
        {
          
            client.poll();

          
            if (!client.connected && currentplayhumanstate != Connecting)
            {
                homeStatusMessage = "Lost connection to the server.";
                currentstate = Home;
                currentplayhumanstate = Connecting;
                break;
            }

            while (!client.incomingMessages.empty())
            {
                json msg = client.incomingMessages.front();
                client.incomingMessages.pop();
                if (!msg.contains("method"))
                    continue;
                std::string method = msg["method"];

               
                try
                {

                if (method == "lobbyFull")
                {
                    lobbyFullMessage = true;
                    homeStatusMessage = "Lobby is full — try again shortly.";
                    currentstate = Home;
                    currentplayhumanstate = Connecting; // reset for next attempt
                }
                else if (method == "updateLobby")
                {
                    lastPlayersConnected = msg["params"]["playersConnected"].get<int>();
                    std::cout << "connected=" << lastPlayersConnected << std::endl;

                    if (lastPlayersConnected == 4 && currentplayhumanstate == Lobby)
                    {
                        std::cout << "Switching to humanShuffling" << std::endl;
                        cardShuffle.Reset();
                        shuffleDelay = SHUFFLE_DELAY;
                        cardsReceived = false;
                        currentplayhumanstate = humanShuffling;
                    }
                }
                else if (method == "getyourcards")
                {
                    std::vector<int> cardIndices = msg["params"]["your_cards"].get<std::vector<int>>();

                   
                    delete players[0];
                    players[0] = new Player(true, false);
                    players[0]->player_id = msg["params"]["player_id"].get<int>();
                    localPlayerId = players[0]->player_id;
                    seatLabels[0] = "You";
                    seatLabels[1] = "Player 2";
                    seatLabels[2] = "Player 3";
                    seatLabels[3] = "Player 4";
                    for (int i = 0; i < 4; i++)
                    {
                        seatBids[i] = 0;
                        seatTricks[i] = 0;
                    }
                    currentTurnPlayerId = -1;
                    myTurn = false;
                    pendingCardIndex = -1;
                    game.timeManager.reset();

                    for (int cardNum : cardIndices)
                        players[0]->receiveCard(cardNum);

                    players[0]->organizeHand();
                    cardsReceived = true;
                    bidAutoSubmitted = false;
                    biddingCompleteReceived = false;
                    bidAutoTimer = 0.0f;
                    
                    bidSubmitted = false;
                    bidScreen.Reset();
                    std::cout << "Received cards from server." << std::endl;
                    // note it arrived, don't transition yet
                }
                else if (method == "biddingComplete")
                {
                    
                    if (msg.contains("params") && msg["params"].contains("bids"))
                    {
                        for (auto &entry : msg["params"]["bids"])
                        {
                            int pid = entry["player_id"].get<int>();
                            int bidVal = entry["bid"].get<int>();
                            if (pid >= 1 && pid <= 4)
                                seatBids[pid - 1] = bidVal;
                        }
                    }

                    //
                    biddingCompleteReceived = true;
                    myTurn = false;
                    pendingCardIndex = -1;
                    humanMoves.clear();

                    if (bidSubmitted)
                    {
                        currentplayhumanstate = humanPlaying;
                    }
                    else
                    {
                        currentplayhumanstate = humanBidding;
                    }
                }
                else if (method == "turnStarted")
                {
                   
                    int turnPlayerId = msg["params"]["player_id"].get<int>();
                    currentTurnPlayerId = turnPlayerId;
                    myTurn = (players[0] != nullptr &&
                              players[0]->player_id == turnPlayerId);
                    if (players[0] != nullptr)
                        localPlayerId = players[0]->player_id;

                    // Every client resets its visual timer at the exact same
                    // network event. Only the active client decrements it.
                    game.timeManager.reset();
                    pendingCardIndex = -1;

                 
                    if (bidSubmitted)
                        currentplayhumanstate = humanPlaying;

                    std::cout << "Timer reset to 60 on turn start." << std::endl;

                    std::cout << "Turn started: Player " << turnPlayerId
                              << (myTurn ? " (YOU)" : "") << std::endl;
                   
                    if (players[0] != nullptr)
                        std::cout << "Local player id = " << players[0]->player_id << std::endl;
                }
                else if (method == "cardPlayed")
                {
                   
                    if (humanTrickEndPending)
                    {
                        humanMoves.clear();
                        humanTrickEndPending = false;
                    }

                    int pid = msg["params"]["player_id"].get<int>();
                    int cardVal = msg["params"]["card"].get<int>();
                    Card c;
                    c.load(cardVal);
                    humanMoves.push_back({pid, c});

                    
                    if (players[0] && pid == players[0]->player_id)
                    {
                        int confirmedIndex = -1;
                        for (int i = 0; i < players[0]->handSize; ++i)
                        {
                            if (players[0]->hand[i].index == cardVal)
                            {
                                confirmedIndex = i;
                                break;
                            }
                        }

                        if (confirmedIndex != -1)
                            players[0]->playCard(confirmedIndex);

                        pendingCardIndex = -1;
                        myTurn = false;
                    }
                }
                else if (method == "invalidMove")
                {
                    // server rejected our pick — let the player try again
                    myTurn = true;
                    pendingCardIndex = -1;
                    game.timeManager.reset();
                    std::cout << "Timer reset after invalid move." << std::endl;
                }
                else if (method == "trickWon")
                {
                    // NEW: tally live tricks-won per seat for the render
                    // interface (previously nothing tracked this client-side).
                    if (msg.contains("params") && msg["params"].contains("player_id"))
                    {
                        int winnerId = msg["params"]["player_id"].get<int>();
                        if (winnerId >= 1 && winnerId <= 4)
                            seatTricks[winnerId - 1]++;
                    }

                  
                    humanTrickEndPending = true;
                    humanTrickEndTimer = HUMAN_TRICK_END_DELAY;
                }
                else if (method == "handOver")
                {
          
                    if (msg.contains("params") && msg["params"].contains("scores"))
                    {
                        for (auto &entry : msg["params"]["scores"])
                        {
                            int pid = entry["player_id"].get<int>();
                            float delta = entry["score"].get<float>();
                            if (pid >= 1 && pid <= 4 && humanHandsPlayed >= 0 && humanHandsPlayed < 5)
                                humanScoresGrid[humanHandsPlayed][pid - 1] = delta;
                        }
                    }

                    humanHandsPlayed++;
                    cardsReceived = false;
                    dealtcards = false;
                    dealStarted = false;
                    cardsToShow = 0;
                    dealCardTimer = 0.0f;
                    humanMoves.clear();
                    myTurn = false;
                    currentTurnPlayerId = -1;
                    pendingCardIndex = -1;
                    game.timeManager.reset();
                    std::cout << "Timer reset after hand over." << std::endl;
                    bidSubmitted = false;
                    bidAutoSubmitted = false;
                    biddingCompleteReceived = false;
                    bidAutoTimer = 0.0f;
                    bidScreen.Reset();
                    cardShuffle.Reset();
                    shuffleDelay = SHUFFLE_DELAY;
                    humanScoreDelay = HUMAN_SCORE_DELAY;
                    currentplayhumanstate = displayscoresHuman;
                }
                else if (method == "gameOver")
                {
             
                    if (msg.contains("params") && msg["params"].contains("scores"))
                    {
                        for (auto &entry : msg["params"]["scores"])
                        {
                            int pid = entry["player_id"].get<int>();
                            float delta = entry["score"].get<float>();
                            if (pid >= 1 && pid <= 4 && humanHandsPlayed >= 0 && humanHandsPlayed < 5)
                                humanScoresGrid[humanHandsPlayed][pid - 1] = delta;
                        }
                    }

                    humanMoves.clear();
                    myTurn = false;
                    currentTurnPlayerId = -1;
                    pendingCardIndex = -1;
                    humanScoreDelay = HUMAN_SCORE_DELAY;
                    matchOverPendingHome = true;
                    currentplayhumanstate = displayscoresHuman;
                }
                
                else if (method == "gameAborted")
                {
                    homeStatusMessage = "Match ended — a player disconnected.";
                    myTurn = false;
                    currentTurnPlayerId = -1;
                    pendingCardIndex = -1;
                    humanMoves.clear();
                    currentstate = Home;
                    currentplayhumanstate = Connecting;
                }

                } // end try
                catch (const std::exception &e)
                {
                    std::cerr << "[ERROR] Failed to process message (method=" << method
                              << "): " << e.what() << " — raw=" << msg.dump() << std::endl;
                }
            }

            if (currentstate != PlayHuman)
                break; // just got rejected / game over, don't fall through

            switch (currentplayhumanstate)
            {
            case Connecting:
            {
                if (client.connected)
                    currentplayhumanstate = Lobby;
                break;
            }

            case Lobby:
            {
                // nothing to do here besides wait for updateLobby, handled above
                break;
            }

            case displayscoresHuman:
            {
                humanScoreDelay -= GetFrameTime();
                if (humanScoreDelay <= 0.0f)
                {
                    humanScoreDelay = HUMAN_SCORE_DELAY;
                    if (matchOverPendingHome)
                    {
                    
                        matchOverPendingHome = false;
                        currentstate = Home;
                        currentplayhumanstate = Connecting;
                        humanHandsPlayed = 0;
                        for (int r = 0; r < 5; r++)
                            for (int c = 0; c < 4; c++)
                                humanScoresGrid[r][c] = 0.0f;
                    }
                    else
                    {
                        currentplayhumanstate = humanShuffling;
                    }
                }
                break;
            }

            case humanShuffling:
            {
                if (cardShuffle.IsSkipButtonPressed())
                    cardShuffle.Skip();

                cardShuffle.Update();
                if (cardShuffle.isDone() && cardsReceived)
                {
                    currentplayhumanstate = humanDealing;
                    dealStarted = false;
                    cardsToShow = 0;
                    dealCardTimer = 0.0f;
                    humanDealingTimer = 0.0f;
                }
                break;
            }

            case humanDealing:
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
                {
                    cardsToShow++;
                    dealCardTimer = 0.0f;
                }

                if (dealAnim.isDone() || humanDealingTimer >= HUMAN_DEALING_FALLBACK_DELAY)
                {
                    // NEW: go pick a bid before playing, instead of
                    // straight into humanPlaying.
                    bidSubmitted = false;
                    bidAutoTimer = 0.0f;
                    bidScreen.Reset();
                    currentplayhumanstate = humanBidding;
                }
                break;
            }

    
            case humanBidding:
            {
                if (!bidSubmitted)
                {
                    bidScreen.Update();
                    bidAutoTimer += GetFrameTime();

                   
                    bool realConfirm = bidScreen.confirmed && bidAutoTimer >= BID_SCREEN_MIN_TIME;

                    if (realConfirm)
                    {
                        int bid = bidScreen.GetSelectedBid();
                        players[0]->bid = bid;
                        json bidMsg = {{"method", "submitBid"}, {"params", {{"bid", bid}}}};
                        client.send(bidMsg);
                        bidSubmitted = true;
                        bidAutoSubmitted = true;
                        bidAutoTimer = 0.0f;
                    }
                    else if (bidAutoTimer >= BID_AUTO_SUBMIT_DELAY)
                    {
                        int fallbackBid = players[0]->chooseBid();
                        players[0]->bid = fallbackBid;
                        json bidMsg = {{"method", "submitBid"}, {"params", {{"bid", fallbackBid}}}};
                        client.send(bidMsg);
                        bidSubmitted = true;
                        bidAutoSubmitted = true;
                        bidAutoTimer = 0.0f;
                        std::cout << "Auto-submitted bid: " << fallbackBid << std::endl;
                    }
                }

                if (bidSubmitted && biddingCompleteReceived)
                {
                    currentplayhumanstate = humanPlaying;
                }
                break;
            }

            case humanPlaying:
            {
               
                if (humanTrickEndPending)
                {
                    humanTrickEndTimer -= GetFrameTime();
                    if (humanTrickEndTimer <= 0)
                    {
                        humanMoves.clear();
                        humanTrickEndPending = false;
                    }
                }

           
                refreshCardRects(players[0]);

                const bool timerShouldRun = myTurn && currentTurnPlayerId > 0 && localPlayerId > 0 && currentTurnPlayerId == localPlayerId;

                if (timerShouldRun)
                {
                    game.timeManager.update(GetFrameTime());

                    if (game.timeManager.currentTime <= 0.0f)
                    {
                        int fallbackIndex = 0;
                        if (fallbackIndex >= 0 && fallbackIndex < players[0]->handSize)
                        {
                            pendingCardIndex = fallbackIndex;
                            json playMsg = {
                                {"method", "playCard"},
                                {"params", {{"card", players[0]->hand[fallbackIndex].index}}}
                            };
                            client.send(playMsg);
                            myTurn = false;
                            game.timeManager.reset();
                            std::cout << "Local timer reset after timeout play." << std::endl;
                        }
                    }
                    else
                    {
                        int idx = players[0]->getCardIndex("", 0, false);
                        if (idx >= 0 && idx < players[0]->handSize)
                        {
                            pendingCardIndex = idx;
                            json playMsg = {
                                {"method", "playCard"},
                                {"params", {{"card", players[0]->hand[idx].index}}}
                            };
                            client.send(playMsg);
                            myTurn = false;
                            game.timeManager.reset();
                            std::cout << "Local timer reset after play." << std::endl;
                        }
                    }
                }
                else
                {
                    game.timeManager.reset();
                    std::cout << "Local timer reset (not my turn)." << std::endl;
                }
                break;
            }

            default:
                break;
            } // end switch(currentplayhumanstate)
            break;
        } // end case PlayHuman
        } // end switch(currentstate)

        // ── Draw ──
        BeginDrawing();

        switch (currentstate)
        {
        case Home:
        {
            ClearBackground(RED);
            HOME.Homedesign_draw();
            if (!homeStatusMessage.empty())
            {
                int fontSize = 22;
                int textWidth = MeasureText(homeStatusMessage.c_str(), fontSize);
                DrawText(homeStatusMessage.c_str(),
                         (GetScreenWidth() - textWidth) / 2, 20, fontSize, WHITE);
            }
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
              
                std::string botLabels[4] = {"You", "Bot 1", "Bot 2", "Bot 3"};
                int botBids[4];
                int botTricks[4];
                for (int i = 0; i < 4; i++)
                {
                    botBids[i] = players[i]->bid;
                    botTricks[i] = players[i]->tricksWon;
                }

                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, players[0]->rects,
                                             game.roundManager.moves, game.timeManager.currentTime,
                                             game.handsPlayed + 1, botLabels, -1, -1, botBids, botTricks);
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
            }
            break;
        }

        case PlayHuman:
        {
            switch (currentplayhumanstate)
            {
            case Connecting:
            {
                ClearBackground({22, 82, 42, 255});
                break;
            }
            case Lobby:
            {
                ClearBackground(BLACK);
                lobbyScreen.Draw(lastPlayersConnected);
                break;
            }
            case displayscoresHuman:
            {
                ClearBackground(BLACK);
            
                score.Draw(humanScoresGrid);
                break;
            }
            case humanShuffling:
            {
                ClearBackground(BLACK);
                cardShuffle.Draw();
                break;
            }
            case humanDealing:
            {
                ClearBackground(BLACK);
                renderer.drawWholeInterface(players[0]->hand, cardsToShow, humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId);
                dealAnim.draw(renderer);
                break;
            }
           
            case humanBidding:
            {
                ClearBackground(BLACK);
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId);
                if (!bidSubmitted)
                {
                    bidScreen.Draw();
                }
                else
                {
                    const char *waitMsg = "Waiting for other players to bid...";
                    DrawTextEx(font,waitMsg, {(float(GetScreenWidth()) - MeasureText(waitMsg, 24)) / 2, 600}, 36,2, GOLD);
                }
                break;
            }
            case humanPlaying:
            {
                ClearBackground(BLACK);
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, players[0]->rects,
                                             humanMoves, game.timeManager.currentTime,
                                             humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId,
                                             seatBids, seatTricks);
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }
        } // end draw switch(currentstate)

        musicHandler.Draw();
        EndDrawing();
    } // end while(!WindowShouldClose())

    for (int i = 0; i < 4; i++)
        delete players[i];
    cardShuffle.Unload();
    HOME.unload();

    CloseAudioDevice();
    CloseWindow();
    UnloadFont(font);

    return 0;
}