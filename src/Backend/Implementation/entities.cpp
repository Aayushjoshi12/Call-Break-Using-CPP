#include "entities.h"
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
using namespace std;
Card::Card() : suit("spades"), value(0), index(0) {
    texture = {0};
}


void Card::load(int cardNum) {
    int n = (cardNum - 1) / 13;
    value = (cardNum - 1) % 13 + 2;
    string suits_array[4]={"spades", "clubs", "hearts", "diamonds"};
    suit=suits_array[n];

    string name = to_string(value) + "_of_" + suit + ".png";

    texture = LoadTexture(("../Assets/Image files/cards/" + name).c_str());
    index = cardNum;
}
void Player::receiveCard(int cardNum)
{
    if (handSize < 13)
    {
        hand[handSize].load(cardNum);
        handSize++;
    }
}
void Card::unload() {
    if (texture.id > 0)
        UnloadTexture(texture);
}



// ───────── DECK ─────────
Deck::Deck() {
    srand((unsigned)time(0));
    for (int i = 0; i < 52; i++)
        raw[i] = i + 1;
}

void Deck::shuffle() {
    for (int i = 51; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(raw[i], raw[j]);
    }
}

int Deck::cardAt(int i) {
    return raw[i];
}

// ───────── PLAYER ─────────
Player::Player(bool ishuman) {
    handSize = 0;
    bid = tricksWon = score = 0;
    isHuman = ishuman;
}