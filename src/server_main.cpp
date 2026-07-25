#include "./Network/headerfiles/server.h"
#include "./Backend/headerfiles/entities.h"
#include "./Backend/headerfiles/RoundManager.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;

#define MAX_PLAYERS 4
#define TURN_TIME 60.0

// The server is authoritative for the game state.  Clients only request a
// card; the server validates it, removes it from the server-side hand, and
// broadcasts the accepted move to every client.
enum GameState
{
    LOBBY,
    BIDDING,
    WAITING_BIDS,
    PLAYING,
    GAME_OVER
};

void scoreHand(Player *players[4], float scores[5][4], int handsPlayed)
{
    for (int i = 0; i < 4; i++)
    {
        if (players[i]->tricksWon < players[i]->bid)
            scores[handsPlayed][i] = -players[i]->bid;
        else
            scores[handsPlayed][i] = players[i]->bid + (1 / 10.0f) *
                                      (players[i]->tricksWon - players[i]->bid);

        players[i]->score += scores[handsPlayed][i];
    }

    for (int i = 0; i < 4; i++)
    {
        players[i]->tricksWon = 0;
        players[i]->bid = 0;
        players[i]->handSize = 0;
        players[i]->networkCardChoice = -1;
    }
}

int main()
{
    Server server;

    if (!server.init(54000, 10))
    {
        cerr << "Failed to initialize server. Exiting." << endl;
        return -1;
    }

    cout << "Server initialized. Entering main loop." << endl;

    GameState state = LOBBY;
    Player *players[MAX_PLAYERS] = {nullptr, nullptr, nullptr, nullptr};
    RoundManager roundManager;

    bool isdeckcreated = false;
    int roundsPlayed = 0;
    int handsPlayed = 0;
    const int TOTAL_HANDS = 5;
    float scores[5][4] = {};

    // Bidding state for the current hand.
    bool bidSubmitted[4] = {false, false, false, false};
    int bidsIn = 0;
    chrono::steady_clock::time_point biddingStartTime = chrono::steady_clock::now();
    const double BIDDING_TIMEOUT_SECONDS = 4.0;

    // Authoritative server-side turn timer.  This prevents the entire game
    // from freezing if a client does not click, loses focus, or disconnects
    // while it is that client's turn.
    chrono::steady_clock::time_point turnStartTime = chrono::steady_clock::now();

    // Every time the server starts a turn, ALL clients receive the same
    // message.  The active client uses it to enable input; all clients use
    // it to reset their visual timer at the same moment.
    auto broadcastTurnStarted = [&]()
    {
        if (roundManager.currentPlayer < 0 || roundManager.currentPlayer >= 4)
            return;

        int playerId = players[roundManager.currentPlayer]->player_id;
        turnStartTime = chrono::steady_clock::now();

        server.broadcast({
            {"method", "turnStarted"},
            {"params", {
                {"player_id", playerId},
                {"duration", TURN_TIME}
            }}}
        );
    };

    // Server-side version of one accepted card play.  Both a normal client
    // request and a timeout use this exact path, so they cannot get out of
    // sync with each other.
    auto processPlay = [&](int senderId, int cardValue, bool timeoutPlay) -> bool
    {
        if (roundManager.currentPlayer < 0 || roundManager.currentPlayer >= 4)
            return false;

        Player *sender = players[roundManager.currentPlayer];
        if (sender == nullptr || sender->player_id != senderId)
            return false;

        int handIndex = -1;
        for (int i = 0; i < sender->handSize; i++)
        {
            if (sender->hand[i].index == cardValue)
            {
                handIndex = i;
                break;
            }
        }

        if (handIndex == -1)
        {
            if (!timeoutPlay)
            {
                server.sendTo(server.peers[roundManager.currentPlayer],
                              {{"method", "invalidMove"}});
            }
            return false;
        }

        sender->networkCardChoice = handIndex;
        bool played = roundManager.updateRound(players);

        if (!played)
        {
            if (!timeoutPlay)
            {
                server.sendTo(server.peers[roundManager.currentPlayer],
                              {{"method", "invalidMove"}});
            }
            return false;
        }

        Move &lastMove = roundManager.moves.back();

        // This is the single source of truth for the table. Every client,
        // including the player who clicked, receives this event.
        server.broadcast({
            {"method", "cardPlayed"},
            {"params", {
                {"player_id", lastMove.player_id},
                {"card", lastMove.cardPlayed.index}
            }}}
        );

        if (timeoutPlay)
        {
            cout << "[TIMEOUT] Player " << senderId
                 << " automatically played card " << lastMove.cardPlayed.index
                 << endl;
        }

        if (roundManager.finished)
        {
            server.broadcast({
                {"method", "trickWon"},
                {"params", {{"player_id", roundManager.winner}}}}
            );

            roundsPlayed++;

            if (roundsPlayed < 13)
            {
                // The winner of the previous trick leads the next one.
                roundManager.startRound(roundManager.winner, players);
                broadcastTurnStarted();
            }
            else
            {
                // One complete 13-trick hand is finished.
                scoreHand(players, scores, handsPlayed);
                handsPlayed++;
                roundsPlayed = 0;

                json scoreJson = json::array();
                for (int i = 0; i < 4; i++)
                    scoreJson.push_back(players[i]->score);

                if (handsPlayed < TOTAL_HANDS)
                {
                    isdeckcreated = false;
                    bidsIn = 0;
                    for (int i = 0; i < 4; i++)
                        bidSubmitted[i] = false;

                    server.broadcast({
                        {"method", "handOver"},
                        {"params", {{"scores", scoreJson}}}}
                    );

                    state = BIDDING;
                }
                else
                {
                    server.broadcast({
                        {"method", "gameOver"},
                        {"params", {{"scores", scoreJson}}}}
                    );

                    state = GAME_OVER;
                }
            }
        }
        else if (state == PLAYING)
        {
            // Advance the shared turn timer for every client immediately after
            // a successful card play so the next player is prompted without
            // waiting for a separate round-end event.
            broadcastTurnStarted();
        }

        return true;
    };

    while (true)
    {
        server.poll(players);

        switch (state)
        {
        case LOBBY:
        {
            if (server.players_connected == 4)
            {
                state = BIDDING;
                roundsPlayed = 0;
                cout << "All players connected. Starting game." << endl;
            }
            break;
        }

        case BIDDING:
        {
            if (!isdeckcreated)
            {
                Deck deck;
                deck.shuffle();

                for (int i = 0; i < 4; i++)
                {
                    players[i]->handSize = 0;
                    players[i]->networkCardChoice = -1;
                    for (int j = 0; j < 13; j++)
                        players[i]->receiveCard(deck.cardAt(i * 13 + j));

                    players[i]->organizeHand();
                }

                for (int i = 0; i < 4; i++)
                {
                    vector<int> handcard;
                    for (int j = 0; j < players[i]->handSize; j++)
                        handcard.push_back(players[i]->hand[j].index);

                    json payload = {
                        {"method", "getyourcards"},
                        {"params", {
                            {"your_cards", handcard},
                            {"player_id", players[i]->player_id}
                        }}}
                    ;

                    server.sendTo(server.peers[i], payload);
                }

                isdeckcreated = true;
                state = WAITING_BIDS;
                biddingStartTime = chrono::steady_clock::now();
                cout << "Cards dealt. Waiting for all bids." << endl;
            }
            break;
        }

        case WAITING_BIDS:
        {
            while (!server.incomingMessages.empty())
            {
                auto front = server.incomingMessages.front();
                int senderId = front.first;
                json msg = front.second;
                server.incomingMessages.pop();

                if (!msg.contains("method") || msg["method"] != "submitBid")
                    continue;

                for (int i = 0; i < 4; i++)
                {
                    if (players[i] && players[i]->player_id == senderId && !bidSubmitted[i])
                    {
                        int bid = msg["params"]["bid"].get<int>();

                        if (bid < 1)
                            bid = 1;
                        if (bid > players[i]->handSize)
                            bid = players[i]->handSize;

                        players[i]->bid = bid;
                        bidSubmitted[i] = true;
                        bidsIn++;

                        cout << "[INFO] Player " << senderId
                             << " bid " << bid << endl;
                        break;
                    }
                }
            }

            double biddingElapsed = chrono::duration<double>(chrono::steady_clock::now() - biddingStartTime).count();
            if (bidsIn < 4 && biddingElapsed >= BIDDING_TIMEOUT_SECONDS)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (players[i] && !bidSubmitted[i])
                    {
                        int fallbackBid = max(1, min(players[i]->handSize, players[i]->chooseBid()));
                        players[i]->bid = fallbackBid;
                        bidSubmitted[i] = true;
                        bidsIn++;
                        cout << "[AUTO-BID] Player " << players[i]->player_id << " -> " << fallbackBid << endl;
                    }
                }
            }

            if (bidsIn == 4)
            {
                roundManager.startRound(1, players);
                roundsPlayed = 0;
                state = PLAYING;

                server.broadcast({{"method", "biddingComplete"}});
                broadcastTurnStarted();

                cout << "All bids received. Starting play." << endl;
            }
            break;
        }

        case PLAYING:
        {
            // Server-authoritative timeout.  We choose the first legal card
            // from the current hand, so timeout can never deadlock the game
            // because of an invalid follow-suit choice.
            double elapsed = chrono::duration<double>(
                                  chrono::steady_clock::now() - turnStartTime)
                                  .count();

            if (elapsed >= TURN_TIME)
            {
                if (roundManager.currentPlayer < 0 || roundManager.currentPlayer >= 4 || players[roundManager.currentPlayer] == nullptr)
                {
                    turnStartTime = chrono::steady_clock::now();
                }
                else
                {
                    Player *current = players[roundManager.currentPlayer];
                    int forcedIndex = -1;

                    for (int i = 0; i < current->handSize; i++)
                    {
                        if (roundManager.isValidMove(current, i))
                        {
                            forcedIndex = i;
                            break;
                        }
                    }

                    if (forcedIndex != -1)
                    {
                        int forcedCard = current->hand[forcedIndex].index;
                        processPlay(current->player_id, forcedCard, true);
                    }
                    else
                    {
                        // A valid hand should always have at least one legal
                        // card. Resetting the clock is safer than a tight loop if
                        // the server receives corrupted state.
                        turnStartTime = chrono::steady_clock::now();
                    }
                }
            }

            // Process every queued client message in this frame.  Messages
            // from an old/non-active player are ignored by processPlay.
            while (!server.incomingMessages.empty() && state == PLAYING)
            {
                auto front = server.incomingMessages.front();
                int senderId = front.first;
                json msg = front.second;
                server.incomingMessages.pop();

                if (!msg.contains("method") || msg["method"] != "playCard")
                    continue;

                if (!msg.contains("params") || !msg["params"].contains("card"))
                    continue;

                int cardValue = msg["params"]["card"].get<int>();
                processPlay(senderId, cardValue, false);
            }
            break;
        }

        case GAME_OVER:
        {
            // The current server process can stay alive and wait for a new
            // set of clients. A fresh lobby requires a restart of the server
            // with the current Server implementation, so do not mutate the
            // completed game state here.
            break;
        }
        }

        this_thread::sleep_for(chrono::milliseconds(5));
    }
}
