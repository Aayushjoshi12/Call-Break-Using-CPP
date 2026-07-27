#pragma once
#include "entities.h"
#include <vector>
#include "RoundManager.h"
#include "TimeManager.h"

#include <string>

class GameManager {
public:
    RoundManager roundManager;
    TimeManager timeManager;
    GameManager();
    int handsPlayed;
    const int TOTAL_HANDS;
    bool needNewHand;
    int roundsPlayed = 0;
    bool gameOver = false;
    float botDelay = 0.0f;
    const float BOT_DELAY = 1.0f;
    float roundEndDelay = 0.0f;
    const float ROUND_END_DELAY = 2.0f;
    bool waitingForNextRound = false;

    void updateGame(Player *players[4], float deltaTime);
    void calculateScores(Player* players[4]);
    // Was declared with no initializer — since GameManager is a stack object,
    // any hand not yet played held raw garbage memory, which is exactly the
    // huge scrambled digits shown on the result screen.
    float scores[5][4] = {};
};