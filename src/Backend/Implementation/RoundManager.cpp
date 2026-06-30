#include "RoundManager.h"

void RoundManager::startRound(int player_id, Player *players[4]) {
    moves.clear();
    leadsuit = "";
    currentBestSuit = "";
    currentBest = 0;
    winner = -1; // ← safe default (was 0, which silently pointed to player 0)
    finished = false;

    // ── find the array index of the winning player ──
    currentPlayer = 0; // ← safe default before search (was unset, caused Bot 3 infinite loop)
    for (int i = 0; i < 4; i++) {
        if (players[i]->player_id == player_id) {
            currentPlayer = i;
            break;
        }
    }

    // isaifirstmove means "the AI is leading this trick"
    isaifirstmove = (currentPlayer != 0);
}

int RoundManager::getWinner() {
    for (int i = 0; i < (int)moves.size(); i++) {
        if (moves[i].cardPlayed.suit == currentBestSuit && moves[i].cardPlayed.value == currentBest) {
            return moves[i].player_id;
        }
    }
    return -1;
}
