#include "../headerfiles/entities.h"
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
using namespace std;
Card::Card() : suit("spades"), value(0), index(0) {
    texture = {0};
}


void Card::load(int cardNum) {
    suit_index = (cardNum - 1) / 13;
    value = (cardNum - 1) % 13 + 2;
    string suits_array[4]={"spades", "clubs", "hearts", "diamonds"};
    suit=suits_array[suit_index];

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
void Player::organizeHand()
{
 
  for(int i=0;i<handSize-1;i++){
    for(int j=i+1;j<handSize;j++){
      if((hand[i].suit_index>hand[j].suit_index)||(hand[i].suit_index==hand[j].suit_index && hand[i].value>hand[j].value)){
        swap(hand[i],hand[j]);
      }
    }
  }
}// ───────── PLAYER ─────────
Player::Player(bool ishuman) {
    handSize = 0;
    bid = tricksWon = score = 0;
    isHuman = ishuman;
}
int Player::getCardIndex(string leadSuit, int currentBest, bool isAIFirstMove) {
    if (isHuman) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < handSize; i++) {
            if (CheckCollisionPointRec(mouse, rects[i]) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return i;
            }
        }
        return -1;
    } else {
        if(isAIFirstMove){
            return 0;
            isAIFirstMove=false;

        }
        else{

        // ───────── check if must beat ─────────
        bool mustBeat = false;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == leadSuit && hand[i].value > currentBest) {
                mustBeat = true;
                break;
            }
        }

        // ───────── if must beat, choose smallest winning card ─────────
        if (mustBeat) {
            int best = -1;

            for (int i = 0; i < handSize; i++) {
                if (hand[i].suit == leadSuit && hand[i].value > currentBest) {
                    if (best == -1 || hand[i].value < hand[best].value) {
                        best = i;
                    }
                }
            }

            return best;
        }

        // ───────── follow suit normally ─────────
        int lowestLead = -1;
        int lowestLeadVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == leadSuit) {
                if (hand[i].value < lowestLeadVal) {
                    lowestLeadVal = hand[i].value;
                    lowestLead = i;
                }
            }
        }

        if (lowestLead != -1) {
            return lowestLead;
        }

        // ───────── no lead suit → play smallest spade ─────────
        int smallestTrump = -1;
        int smallestTrumpVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == "spades") {
                if (hand[i].value < smallestTrumpVal) {
                    smallestTrumpVal = hand[i].value;
                    smallestTrump = i;
                }
            }
        }

        if (smallestTrump != -1) return smallestTrump;

        // ───────── fallback ─────────
        int lowestAny = 0;
        int lowestAnyVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].value < lowestAnyVal) {
                lowestAnyVal = hand[i].value;
                lowestAny = i;
            }
        }

        return lowestAny;
     }
    }

}
Card Player::playCard(int index) {
    Card played = hand[index];

    swap(hand[index], hand[handSize - 1]);
    handSize--;

    return played;
}
int Player::chooseBid()
{
    int suitCount[4] = {0, 0, 0, 0};
    double points = 0.0;

    for (int i = 0; i < handSize; i++)
    {
        suitCount[hand[i].suit_index]++;
        bool isSpade = hand[i].suit == "spades";
        int v = hand[i].value;

        if (isSpade)
            points += (v == 14) ? 1.0 : (v == 13) ? 0.9 : (v == 12) ? 0.7 : (v == 11) ? 0.5 : 0.2;
        else if (v >= 12)
            points += (v == 14) ? 1.0 : (v == 13) ? 0.6 : 0.3;
    }

    int spadeCount = suitCount[0];
    if (spadeCount >= 5)
        points += (spadeCount - 4) * 0.5;

    if (spadeCount > 0)
        for (int s = 1; s < 4; s++)
            if (suitCount[s] == 0)
                points += 0.3;

    return min(handSize, max(1, (int)round(points)));
}
