#include "CardShuffle.h"

// ─── Global instances ─────────────────────────────────────────────────────────
CardShuffle cardShuffler;
DealAnimation dealAnim;

// ═══════════════════════════════════════════════════════════════════════════════
// RENDERER
// ═══════════════════════════════════════════════════════════════════════════════

Renderer::Renderer()
{
    cardBack   = LoadTexture("../Assets/Image files/robot-bot-icon.png");
    table      = LoadTexture("../Assets/Image files/table.png");
    background = LoadTexture("../Assets/Image files/BackGround.png");
    mute       = LoadTexture("../Assets/Image files/mute.png");
    font       = LoadFont("../Assets/Fonts/Cinzel-Bold.ttf");
    boldFont   = LoadFont("../Assets/Fonts/Cinzel-Bold.ttf");
    goldColor  = {255, 215, 0, 255};

    for (int i = 0; i < 13; i++)
    {
        string path = "../Assets/Image files/cards/" + to_string(i + 2) + "_of_clubs.png";
        cards[i] = LoadTexture(path.c_str());
    }
}

void Renderer::drawCardBack(float x, float y)
{
    float scale = 70.0f / cardBack.width;
    DrawTextureEx(cardBack, {x - 35.0f, y - (cardBack.height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawTable(int x, int y)
{
    float scale = 800.0f / table.width;
    DrawTextureEx(table, {x - 400.0f, y - (table.height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawCard(int index, int x, int y)
{
    float scale = 50.0f / cards[index].width;
    DrawTextureEx(cards[index], {x - 25.0f, y - (cards[index].height * scale) / 2}, 0.0f, scale, WHITE);
}

void Renderer::drawBackground()
{
    DrawTexturePro(
        background,
        {0, 0, (float)background.width, (float)background.height},
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {0, 0}, 0.0f, WHITE);
}

void Renderer::drawMute(float x, float y)
{
    float scale = 30.0f / mute.width;
    DrawTextureEx(mute, {x - 15.0f, y - 15.0f}, 0.0f, scale, WHITE);
}

void Renderer::drawWholeInterface()
{
    drawBackground();

    // Draw bots
    for (int i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
        {
            drawCardBack(600.0, 100.0);
            Vector2 size = MeasureTextEx(font, "Bot 1", 48, 2);
            float x = 600.0 - size.x / 2;
            float y = 100.0 + 108.72 - size.y - 10;
            DrawTextEx(font, "Bot 1", (Vector2){x, y}, 48, 2, WHITE);
            break;
        }
        case 1:
        {
            drawCardBack(200.0, 400.0);
            Vector2 size = MeasureTextEx(font, "Bot 2", 48, 2);
            float x = 200.0 - size.x / 2;
            float y = 400.0 + 108.72 - size.y - 10;
            DrawTextEx(font, "Bot 2", (Vector2){x, y}, 48, 2, WHITE);
            break;
        }
        case 2:
        {
            drawCardBack(1000.0, 400.0);
            Vector2 size = MeasureTextEx(font, "Bot 3", 48, 2);
            float x = 1000.0 - size.x / 2;
            float y = 400.0 + 108.72 - size.y - 10;
            DrawTextEx(font, "Bot 3", (Vector2){x, y}, 48, 2, WHITE);
            break;
        }
        }
    }

    drawTable(600, 430);
    drawMute(1100.0f, 40.0f);

    Vector2 size = MeasureTextEx(boldFont, "Round 1", 48, 2);
    float x = 150.0 - size.x / 2;
    float y = 50.0 - size.y / 210;
    DrawTextEx(boldFont, "Round 1", (Vector2){x, y}, 48, 2, goldColor);
}

Renderer::~Renderer()
{
    UnloadTexture(cardBack);
    UnloadTexture(table);
    UnloadTexture(background);
    UnloadTexture(mute);
    for (int i = 0; i < 13; i++)
        UnloadTexture(cards[i]);
    UnloadFont(font);
    UnloadFont(boldFont);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEAL ANIMATION
// ═══════════════════════════════════════════════════════════════════════════════

DealAnimation::DealAnimation()
{
    currentCard   = 0;
    dealDelay     = 0.0f;
    dealInterval  = 0.08f;  // seconds between each card launch
    finished      = false;
}

void DealAnimation::start(vector<int> playerHand)
{
    animCards.clear();
    currentCard  = 0;
    dealDelay    = 0.0f;
    finished     = false;
    buildTargets(playerHand);
}

void DealAnimation::buildTargets(vector<int> playerHand)
{
    // Center of screen — all cards start here
    float cx = 600.0f;
    float cy = 400.0f;

    // Deal order: Bot1(top), Bot2(left), Bot3(right), Player(bottom)
    // 4 players x 13 cards = 52 cards total
    // We cycle: bot1, bot2, bot3, player, bot1, bot2 ...

    for (int round = 0; round < 13; round++)
    {
        // Bot 1 — top center, stacked
        AnimatedCard c1;
        c1.startX = cx; c1.startY = cy;
        c1.targetX = 600.0f + round * 2.0f;  // slight offset so stack looks real
        c1.targetY = 100.0f + round * 2.0f;
        c1.x = cx; c1.y = cy;
        c1.progress = 0.0f;
        c1.done = false;
        c1.faceUp = false;
        c1.cardIndex = 0;
        animCards.push_back(c1);

        // Bot 2 — left, stacked
        AnimatedCard c2;
        c2.startX = cx; c2.startY = cy;
        c2.targetX = 200.0f + round * 2.0f;
        c2.targetY = 400.0f + round * 2.0f;
        c2.x = cx; c2.y = cy;
        c2.progress = 0.0f;
        c2.done = false;
        c2.faceUp = false;
        c2.cardIndex = 0;
        animCards.push_back(c2);

        // Bot 3 — right, stacked
        AnimatedCard c3;
        c3.startX = cx; c3.startY = cy;
        c3.targetX = 1000.0f + round * 2.0f;
        c3.targetY = 400.0f + round * 2.0f;
        c3.x = cx; c3.y = cy;
        c3.progress = 0.0f;
        c3.done = false;
        c3.faceUp = false;
        c3.cardIndex = 0;
        animCards.push_back(c3);

        // Player — bottom spread, face up
        AnimatedCard cp;
        cp.startX = cx; cp.startY = cy;
        cp.targetX = 240.0f + round * 60.0f;
        cp.targetY = 700.0f;
        cp.x = cx; cp.y = cy;
        cp.progress = 0.0f;
        cp.done = false;
        cp.faceUp = true;
        cp.cardIndex = playerHand[round] % 13; // map to 0-12
        animCards.push_back(cp);
    }
}

// smooth ease-out curve
static float easeOut(float t)
{
    return 1.0f - (1.0f - t) * (1.0f - t);
}

void DealAnimation::update(float dt)
{
    if (finished) return;

    // Launch next card after interval
    dealDelay += dt;
    if (dealDelay >= dealInterval && currentCard < (int)animCards.size())
    {
        dealDelay = 0.0f;
        currentCard++;
    }

    // Update all launched cards
    bool allDone = true;
    for (int i = 0; i < currentCard && i < (int)animCards.size(); i++)
    {
        AnimatedCard& c = animCards[i];
        if (!c.done)
        {
            c.progress += dt * 3.0f;  // speed: higher = faster
            if (c.progress >= 1.0f)
            {
                c.progress = 1.0f;
                c.done = true;
            }
            float t = easeOut(c.progress);
            c.x = c.startX + (c.targetX - c.startX) * t;
            c.y = c.startY + (c.targetY - c.startY) * t;
        }
        if (!c.done) allDone = false;
    }

    if (currentCard >= (int)animCards.size() && allDone)
        finished = true;
}

void DealAnimation::draw(Renderer& r)
{
    for (int i = 0; i < currentCard && i < (int)animCards.size(); i++)
    {
        AnimatedCard& c = animCards[i];
        if (c.faceUp)
            r.drawCard(c.cardIndex, (int)c.x, (int)c.y);
        else
            r.drawCardBack(c.x, c.y);
    }
}

bool DealAnimation::isDone() const
{
    return finished;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CARDSHUFFLE
// ═══════════════════════════════════════════════════════════════════════════════

CardShuffle::CardShuffle()
{
    initDeck();
}

void CardShuffle::initDeck()
{
    deck.clear();
    for (int i = 0; i < 52; i++)
        deck.push_back(i);
}

void CardShuffle::shuffle()
{
    initDeck();
    random_device rd;
    mt19937 rng(rd());
    std::shuffle(deck.begin(), deck.end(), rng);
}

vector<int> CardShuffle::getShuffledDeck() const
{
    return deck;
}

void CardShuffle::printDeck() const
{
    const string suits[] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    const string ranks[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};

    for (int i = 0; i < (int)deck.size(); i++)
    {
        int suit = deck[i] / 13;
        int rank = deck[i] % 13;
        cout << ranks[rank] << " of " << suits[suit];
        if (i < (int)deck.size() - 1) cout << ", ";
    }
    cout << endl;
}

CardShuffle::~CardShuffle()
{
    // vector auto-destructs
}

// ─── Single callable wrapper ──────────────────────────────────────────────────
void callforshuffling()
{
    cardShuffler.shuffle();
    // give player the first 13 cards
    vector<int> deck = cardShuffler.getShuffledDeck();
    vector<int> playerHand(deck.begin(), deck.begin() + 13);
    dealAnim.start(playerHand);
}