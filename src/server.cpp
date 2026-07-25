#include <iostream>
#include <vector>
using namespace std;
#define MAX_PLAYERS 4
enum GameState
{
    LOBBY,
    IN_GAME,
    GAME_OVER
};

int main()
{
    Server server;

    
    if (!server.init(54000, 10))
    {
        std::cerr << "Failed to initialize server. Exiting." << std::endl;
        return -1;
    }
    std::cout << "Server initialized. Entering main loop." << std::endl;
    GameState state = LOBBY;
    Player *players[MAX_PLAYERS] = {nullptr};
    bool isdeckcreated = false;

    while (true)
    {
        switch (state)
        {
        case LOBBY:
            server.poll(players);
            if (server.players_connected == 4)
            {
                state = IN_GAME;
                std::cout << "All players connected. Starting game." << std::endl;
            }
            break;
        case IN_GAME:
            if (!isdeckcreated)
            {
                Deck deck;
                deck.shuffle();
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 13; j++)
                    {
                        players[i]->receiveCard(deck.cardAt(i * 13 + j));
                    }
                }
                
            for (int i = 0; i < 4; i++)
            {
                vector<int> handcard;
                for (int j = 0; j < 13; j++)
                {
                    handcard.push_back(players[i]->hand[j].index);
                }
                json payload = {
                    {"method", "getyourcards"},
                    {"params", {{"your_cards", handcard}}}};
                server.sendTo(server.peers[i], payload);
            }
            isdeckcreated = true;
        }
          
            server.poll(players);
            break;
        case GAME_OVER:
            server.poll(players);
            break;
        }
    }
}
