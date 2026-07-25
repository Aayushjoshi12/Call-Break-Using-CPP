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
    InitAudioDevice();

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

    // NEW: has this client sent its bid to the server for the current hand?
    bool bidSubmitted = false;
    bool bidAutoSubmitted = false;
    float bidAutoTimer = 0.0f;
    const float BID_AUTO_SUBMIT_DELAY = 3.0f;

    // NEW: trickWon no longer clears humanMoves synchronously (that was
    // wiping the trick-winning card before it was ever drawn, since
    // "cardPlayed" + "trickWon" frequently arrive in the same poll batch).
    // Instead we keep the finished trick on screen for a short delay,
    // and only actually clear it once that delay expires OR the next
    // trick's first card shows up (whichever happens first).
    bool humanTrickEndPending = false;
    float humanTrickEndTimer = 0.0f;
    const float HUMAN_TRICK_END_DELAY = 1.2f;

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
                    if (bidScreen.confirmed)
                    {
                        players[0]->bid = bidScreen.GetSelectedBid();
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
                    bidScreen.confirmed = false;
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
            // FIX: message handling used to live inside each sub-state's own
            // case block, so only the message types that particular case
            // expected got parsed. client.poll() can hand back several
            // queued messages in a single frame though, and only one case
            // runs per frame — e.g. if "biddingComplete" and the leader's
            // "cardPlayed" both arrived in the same batch while a client
            // was still executing the humanBidding case, "cardPlayed" had
            // no handler there and was silently dropped. That card then
            // never made it into humanMoves, so it never showed up on that
            // client's table, and depending on timing the turn handoff
            // could get lost right along with it.
            //
            // Fix: poll and process ALL incoming messages once per frame,
            // regardless of which sub-state we're currently in, so no
            // message type is ever gated behind the "wrong" case. The
            // switch below only handles local input/animation now.
            client.poll();

            while (!client.incomingMessages.empty())
            {
                json msg = client.incomingMessages.front();
                client.incomingMessages.pop();
                if (!msg.contains("method"))
                    continue;
                std::string method = msg["method"];

                if (method == "lobbyFull")
                {
                    lobbyFullMessage = true;
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

                    // isnetworked=false because this player is driven
                    // locally by the mouse; the chosen bid/card is then
                    // sent to the server explicitly.
                    // FIX: use the ID the server actually assigned
                    // instead of hard-coding 1 — hard-coding it broke
                    // every client except whoever happened to connect
                    // first, since their own "cardPlayed" echoes never
                    // matched players[0]->player_id.
                    // A new hand replaces the old local hand. Delete the
                    // previous object first so every client starts the hand
                    // with exactly the cards sent by the server.
                    delete players[0];
                    players[0] = new Player(true, false);
                    players[0]->player_id = msg["params"]["player_id"].get<int>();
                    localPlayerId = players[0]->player_id;
                    seatLabels[0] = "You";
                    seatLabels[1] = "Player 2";
                    seatLabels[2] = "Player 3";
                    seatLabels[3] = "Player 4";
                    currentTurnPlayerId = -1;
                    myTurn = false;
                    pendingCardIndex = -1;
                    game.timeManager.reset();

                    for (int cardNum : cardIndices)
                        players[0]->receiveCard(cardNum);

                    players[0]->organizeHand();
                    cardsReceived = true;
                    bidAutoSubmitted = false;
                    bidAutoTimer = 0.0f;
                    std::cout << "Received cards from server." << std::endl;
                    // note it arrived, don't transition yet
                }
                else if (method == "biddingComplete")
                {
                    // Do not start the local clock here. The server sends a
                    // separate turnStarted event containing the actual
                    // active player, which keeps all clients synchronized.
                    myTurn = false;
                    pendingCardIndex = -1;
                    humanMoves.clear();
                    currentplayhumanstate = humanPlaying;
                }
                else if (method == "turnStarted")
                {
                    // IMPORTANT: the server sends turnStarted to EVERY
                    // client. Previously the server sent turnStarted while
                    // the client was waiting for "yourTurn", so clients never
                    // reliably knew that it was their turn.
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
                    currentplayhumanstate = humanPlaying;
                    std::cout << "Timer reset to 60 on turn start." << std::endl;

                    std::cout << "Turn started: Player " << turnPlayerId
                              << (myTurn ? " (YOU)" : "") << std::endl;
                    std::cout << "Local player id = " << players[0]->player_id << std::endl;
                }
                else if (method == "cardPlayed")
                {
                    // if the previous trick is still being shown, this
                    // card belongs to the new trick — clear the old one
                    // now that we actually have something to replace it
                    // with, instead of on a bare "trickWon" signal.
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

                    // The server is authoritative. Remove the exact card
                    // that the server says was played instead of relying on
                    // a stale array index captured before the network round
                    // trip.
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
                    // FIX: used to clear humanMoves right here, but
                    // "cardPlayed" (the winning card) and "trickWon" often
                    // arrive in the same poll batch, so the trick-winning
                    // card was wiped before a single frame ever drew it.
                    // Just start the display delay instead; the actual
                    // clear happens below (timer) or on the next cardPlayed.
                    humanTrickEndPending = true;
                    humanTrickEndTimer = HUMAN_TRICK_END_DELAY;
                }
                else if (method == "handOver")
                {
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
                    bidAutoTimer = 0.0f;
                    bidScreen.confirmed = false;
                    cardShuffle.Reset();
                    shuffleDelay = SHUFFLE_DELAY;
                    currentplayhumanstate = humanShuffling;
                }
                else if (method == "gameOver")
                {
                    currentstate = Home;
                    currentplayhumanstate = Connecting;
                    humanHandsPlayed = 0;
                    myTurn = false;
                    currentTurnPlayerId = -1;
                    pendingCardIndex = -1;
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

            case humanShuffling:
            {
                cardShuffle.Update();
                if (cardShuffle.isDone() && cardsReceived)
                {
                    currentplayhumanstate = humanDealing;
                    dealStarted = false;
                    cardsToShow = 0;
                    dealCardTimer = 0.0f;
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

                if (dealAnim.isDone())
                {
                    // NEW: go pick a bid before playing, instead of
                    // straight into humanPlaying.
                    bidSubmitted = false;
                    bidScreen.confirmed = false;
                    currentplayhumanstate = humanBidding;
                }
                break;
            }

            // NEW: every client shows BidScreen, submits their bid to the
            // server, then waits until everyone else has too.
            case humanBidding:
            {
                if (!bidSubmitted)
                {
                    bidScreen.Update();
                    bidAutoTimer += GetFrameTime();

                    if (bidScreen.confirmed)
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
                break;
            }

            case humanPlaying:
            {
                // NEW: advance/expire the "keep last trick visible" delay
                // regardless of whose turn it is.
                if (humanTrickEndPending)
                {
                    humanTrickEndTimer -= GetFrameTime();
                    if (humanTrickEndTimer <= 0)
                    {
                        humanMoves.clear();
                        humanTrickEndPending = false;
                    }
                }

                // The countdown should move for everyone so the table stays
                // visually synchronized. The server remains the authority for
                // timeout handling, but the local client should only send a
                // move once it has a valid click and the turn is clearly its own.
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
                    // Keep the timer frozen for non-active seats so the countdown
                    // only runs for the client whose turn it is.
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
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, players[0]->rects,
                                             game.roundManager.moves, game.timeManager.currentTime,
                                             game.handsPlayed + 1);
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
                ClearBackground({22, 82, 42, 255});
                lobbyScreen.Draw(lastPlayersConnected);
                break;
            }
            case humanShuffling:
            {
                ClearBackground({22, 82, 42, 255});
                cardShuffle.Draw();
                break;
            }
            case humanDealing:
            {
                ClearBackground({22, 82, 42, 255});
                renderer.drawWholeInterface(players[0]->hand, cardsToShow, humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId);
                dealAnim.draw(renderer);
                break;
            }
            // NEW: draw the bid screen (or a "waiting" message once this
            // client has already submitted) over the dealt hand.
            case humanBidding:
            {
                ClearBackground({22, 82, 42, 255});
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId);
                if (!bidSubmitted)
                {
                    bidScreen.Draw();
                }
                else
                {
                    const char *waitMsg = "Waiting for other players to bid...";
                    DrawText(waitMsg, (GetScreenWidth() - MeasureText(waitMsg, 24)) / 2, 700, 24, WHITE);
                }
                break;
            }
            case humanPlaying:
            {
                ClearBackground({22, 82, 42, 255});
                renderer.drawWholeInterface(players[0]->hand, players[0]->handSize, players[0]->rects,
                                             humanMoves, game.timeManager.currentTime,
                                             humanHandsPlayed + 1, seatLabels, currentTurnPlayerId, localPlayerId);
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

    return 0;
}