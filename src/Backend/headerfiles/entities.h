#pragma once

#include <string>

#include "raylib.h"

class Card {
public:
    std::string suit;
    int value;
    int suit_index;
    Texture2D texture;
    int index;
   

    Card();

    void load(int cardNum);
    void unload();

};

class Deck {
public:
    int raw[52];

    Deck();

    void shuffle();
    int cardAt(int i);
};
class Player {
public:
   
    std::string name;
    int player_id;
    Card hand[13];
    int handSize;
    bool isHuman;
    Rectangle rects[13];
    Player();
    Player(bool ishuman);   
    int getCardIndex(std::string leadSuit, int currentBest,bool isAIFirstMove);
    void organizeHand();  
    void receiveCard(int cardNum);
    void unloadCards();
    void getPlayedCard();
    int chooseBid();
    Card playCard(int index);
    int bid;
    int tricksWon;
    float score;
    
};
struct Move {
    int player_id;
    Card cardPlayed;
};
