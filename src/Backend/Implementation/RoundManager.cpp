#include"../headerfiles/RoundManager.h"
#include"../headerfiles/entities.h"
#include <iostream>
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
bool RoundManager::updateRound(Player *players[4])
{
    if (finished)
        return false;

    Player *current = players[currentPlayer];

    // isaifirstmove is only true for the trick leader if they are AI
    // update it per-move so mid-trick bots get correct info
    bool isLeading = moves.empty();
    bool isAILeading = isLeading && (currentPlayer != 0);

    int card_index = current->getCardIndex(leadsuit, currentBest, isAILeading);
    if (card_index == -1)
        return false;

    if (!isValidMove(current, card_index))
        return false;

    Card played = current->playCard(card_index);

    // ── update best card tracking ──
    if (moves.empty())
    {
        leadsuit = played.suit;
        currentBestSuit = leadsuit;
        currentBest = played.value;
    }
    else if (played.suit == "spades" && currentBestSuit != "spades")
    {
        currentBestSuit = "spades";
        currentBest = played.value;
    }
    else if (played.suit == currentBestSuit && played.value > currentBest)
    {
        currentBest = played.value;
    }

    moves.push_back({current->player_id, played});
    currentPlayer = (currentPlayer + 1) % 4;

    // ── trick complete ──
    if (moves.size() == 4)
    {
        finished = true;
        winner = getWinner();
        if (winner != -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (players[i]->player_id == winner)
                {
                    players[i]->tricksWon++;
                    break;
                }
            }
        }
        else
        {
            // fallback: should never happen, but log if it does
            std::cout << "[ERROR] getWinner() returned -1! currentBestSuit="
                      << currentBestSuit << " currentBest=" << currentBest << std::endl;
        }
    }

    return true;
}
bool RoundManager::isValidMove(Player *p, int cardIndex)
{
    if (leadsuit.empty())
        return true;

    if (p->hand[cardIndex].suit == leadsuit)
    {
        // must beat if possible
        bool canBeat = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == leadsuit && p->hand[i].value > currentBest)
            {
                canBeat = true;
                break;
            }
        }
        if (canBeat)
        {
            return p->hand[cardIndex].value > currentBest;
        }
        else
        {
            return true; // can't beat, any lead suit card is fine
        }
    }
    else
    {
        // playing off-suit — only allowed if player has no lead suit
        bool hasLeadSuit = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == leadsuit)
            {
                hasLeadSuit = true;
                break;
            }
        }

        if (hasLeadSuit)
        {
            return false; // must follow suit
        }

        // no lead suit — must play spade if they have one
        bool hasSpades = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == "spades")
            {
                hasSpades = true;
                break;
            }
        }

        if (hasSpades)
        {
            return p->hand[cardIndex].suit == "spades";
        }

        return true; // no lead suit, no spades — play anything
    }
}

int RoundManager::getWinner() {
    for (int i = 0; i < (int)moves.size(); i++) {
        if (moves[i].cardPlayed.suit == currentBestSuit && moves[i].cardPlayed.value == currentBest) {
            return moves[i].player_id;
        }
    }
    return -1;
}