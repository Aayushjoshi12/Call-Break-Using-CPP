#pragma once

#include <string>

#include "raylib.h"

class Card {
public:
    std::string suit;
    int value;
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