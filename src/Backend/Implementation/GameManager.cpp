#include"../headerfiles/entities.h"
#include"../headerfiles/GameManager.h"
#include"../headerfiles/RoundManager.h"
#include"../headerfiles/TimeManager.h"
#include<iostream>

GameManager::GameManager() : TOTAL_HANDS(2), handsPlayed(0), needNewHand(false) {}
// ───────── GAMEMANAGER ─────────
void GameManager::updateGame(Player *players[4])
{
    if (gameOver)
        return;

    // ── waiting between tricks ──
    if (waitingForNextRound)
    {
        roundEndDelay -= GetFrameTime();
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
        timeManager.update();
        if (timeManager.currentTime <= 0)
        {
            // time expired — force AI to pick for the human (play card 0)
            roundManager.updateRound(players);
            timeManager.reset();
        }
        else
        {
            // wait for actual mouse click inside getCardIndex
            bool played = roundManager.updateRound(players);
            if (played)
                timeManager.reset();
        }
    }
    else
    {
        // ── bot turn ──
        botDelay -= GetFrameTime();
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