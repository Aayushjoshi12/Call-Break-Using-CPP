#include"../headerfiles/entities.h"
#include"../headerfiles/GameManager.h"
#include"../headerfiles/RoundManager.h"
#include"../headerfiles/TimeManager.h"
#include<iostream>

GameManager::GameManager() : TOTAL_HANDS(5), handsPlayed(0), needNewHand(false) {}
// ───────── GAMEMANAGER ─────────
void GameManager::updateGame(Player *players[4])
{
    if (gameOver)
        return;

    // ── waiting between tricks ──
    if (waitingForNextRound)
    {
        roundEndDelay -= deltaTime;
        if (roundEndDelay <= 0)
        {
            waitingForNextRound = false;
            roundsPlayed++;
            if (roundsPlayed < 13)
            {
                roundManager.startRound(roundManager.winner, players);
                timeManager.reset();
                botDelay = BOT_DELAY;
            }
            else
            {
                calculateScores(players); // adds this hand's result to running score
                handsPlayed++;

                if (handsPlayed < TOTAL_HANDS)
                {
                    // hand finished, but match continues
                    roundsPlayed = 0;
                    needNewHand = true; // main.cpp will reshuffle and redeal
                }
                else
                {
                    gameOver = true; // match (all 5 hands) finished
                }
            }
        }
        return; // do nothing else while waiting
    }

    // ── normal play ──
    if (roundManager.currentPlayer == 0)
    {
        // ── human turn: only act on mouse click, not every frame ──
        timeManager.update(deltaTime);
        if (timeManager.currentTime <= 0)
        {
            // Timeout must choose a LEGAL card.  Playing index 0 blindly can
            // violate the follow-suit rule and leave the round stuck forever.
            Player *human = players[roundManager.currentPlayer];
            int forcedIndex = -1;

            for (int i = 0; i < human->handSize; ++i)
            {
                if (roundManager.isValidMove(human, i))
                {
                    forcedIndex = i;
                    break;
                }
            }

            if (forcedIndex != -1)
            {
                human->networkCardChoice = forcedIndex;
                if (roundManager.updateRound(players))
                    timeManager.reset();
            }
            else
            {
                // This should only be reachable with a corrupted/empty hand.
                // Give the game another frame instead of spinning at zero.
                timeManager.reset();
            }
        }
        else
        {
            // Wait for actual mouse click inside getCardIndex.
            bool played = roundManager.updateRound(players);
            if (played)
                timeManager.reset();
        }
    }
    else
    {
        // ── bot turn ──
        botDelay -= deltaTime;
        if (botDelay <= 0)
        {
            bool played = roundManager.updateRound(players);
            if (played)
            {
                botDelay = BOT_DELAY;
            }
            else
            {
                // move failed (invalid card from AI), retry quickly
                botDelay = 0.1f;
                std::cout << "[WARN] Bot " << roundManager.currentPlayer
                          << " failed to play, retrying..." << std::endl;
            }
        }
    }

    // ── trick just finished ──
    if (roundManager.finished && !waitingForNextRound)
    {
        waitingForNextRound = true;
        roundEndDelay = ROUND_END_DELAY;
    }
}
//Prakrit

void GameManager::calculateScores(Player *players[4])
{
    for (int i = 0; i < 4; i++)
    {
        if (players[i]->tricksWon < players[i]->bid)
        {
            scores[handsPlayed][i] = -players[i]->bid;
            players[i]->score += scores[handsPlayed][i];
           
        }
        else
        {
            scores[handsPlayed][i] = players[i]->bid + (1/10.0f)*(players[i]->tricksWon-players[i]->bid);
            players[i]->score += scores[handsPlayed][i];
            
        }
    }

 for (int i = 0; i < 4; i++) {
    players[i]->tricksWon = 0;
    players[i]->bid = 0;
    players[i]->handSize = 0;
}
}
