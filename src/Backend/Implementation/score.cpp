#include<iostream>
calculateScores(Player *players[4])
{
    for (int i = 0; i < 4; i++)
    {
        if (players[i]->tricksWon < players[i]->bid)
        {
            players[i]->score -= players[i]->bid;
        }
        else
        {
            players[i]->score += players[i]->tricksWon;
        }
    }
 for (int i = 0; i < 4; i++) {
    players[i]->tricksWon = 0;
    players[i]->bid = 0;
    players[i]->unloadCards();   
}
};


