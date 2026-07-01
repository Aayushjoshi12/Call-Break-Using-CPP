#ifndef ROUNDMANAGER_H
#define ROUNDMANAGER_H

#include <vector>
#include <string>
using namespace std;

class RoundManager {
public:
    vector<Move> moves;
    string leadsuit = "";
    string currentBestSuit = "";
    int currentBest = 0;
    int currentPlayer = 0;
    bool isaifirstmove = false;
    int winner = -1;
    bool finished = false;

    void startRound(int player_id, Player players[4]);
    bool isValidMove(Player p, int cardIndex);
    bool updateRound(Player *players[4]);
    int getWinner();
};

#endif
