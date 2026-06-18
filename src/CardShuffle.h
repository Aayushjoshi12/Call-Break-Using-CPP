#pragma once
#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

// ─── Renderer ────────────────────────────────────────────────────────────────
class Renderer
{
public:
    Renderer();
    ~Renderer();

    void drawCardBack(float x, float y);
    void drawTable(int x, int y);
    void drawCard(int index, int x, int y);
    void drawBackground();
    void drawMute(float x, float y);
    void drawWholeInterface();

private:
    Texture2D cardBack;
    Texture2D table;
    Texture2D background;
    Texture2D mute;
    Texture2D cards[13];
    Font font;
    Font boldFont;
    Color goldColor;
};

// ─── AnimatedCard ─────────────────────────────────────────────────────────────
struct AnimatedCard
{
    float x, y;           // current position
    float targetX, targetY; // destination
    float startX, startY;   // origin (center)
    float progress;         // 0.0 to 1.0
    bool  done;
    bool  faceUp;           // true = show card, false = show back
    int   cardIndex;        // which card texture to draw
};

// ─── DealAnimation ───────────────────────────────────────────────────────────
class DealAnimation
{
public:
    DealAnimation();

    void start(vector<int> playerHand);   // call once to begin
    void update(float dt);                // call every frame
    void draw(Renderer& r);               // call every frame
    bool isDone() const;                  // true when all cards dealt

private:
    vector<AnimatedCard> animCards;
    int   currentCard;    // which card is currently flying
    float dealDelay;      // timer between cards
    float dealInterval;   // seconds between each card launch
    bool  finished;

    // target positions
    // player: bottom center spread
    // bot1: top center stacked
    // bot2: left stacked
    // bot3: right stacked
    void buildTargets(vector<int> playerHand);
};

// ─── CardShuffle ─────────────────────────────────────────────────────────────
class CardShuffle
{
public:
    CardShuffle();
    ~CardShuffle();

    void shuffle();
    vector<int> getShuffledDeck() const;
    void printDeck() const;

private:
    vector<int> deck;
    void initDeck();
};

// ─── Global instances & wrappers ─────────────────────────────────────────────
extern CardShuffle cardShuffler;
extern DealAnimation dealAnim;

void callforshuffling();