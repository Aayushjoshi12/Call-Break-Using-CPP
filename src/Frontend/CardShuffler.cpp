#include "CardShuffler.h"

// ═══════════════════════════════════════════════════════════════════════════════
// DEAL ANIMATION
// ═══════════════════════════════════════════════════════════════════════════════

DealAnimation::DealAnimation()
{
    currentCard  = 0;
    dealDelay    = 0.0f;
    dealInterval = 0.08f;
    finished     = false;
}

void DealAnimation::start(const Card* hand, int count)
{
    animCards.clear();
    currentCard = 0;
    dealDelay   = 0.0f;
    finished    = false;
    buildTargets(hand, count);
}

void DealAnimation::buildTargets(const Card* hand, int count)
{
    float cx = 600.0f;
    float cy = 400.0f;

    for (int round = 0; round < count; round++)
    {
        // Bot 1 — top center, stacked, face down
        AnimatedCard c1;
        c1.startX = cx; c1.startY = cy;
        c1.targetX = 600.0f + round * 2.0f;
        c1.targetY = 100.0f + round * 2.0f;
        c1.x = cx; c1.y = cy;
        c1.progress = 0.0f;
        c1.done = false;
        c1.faceUp = false;
        c1.card = nullptr;
        animCards.push_back(c1);

        // Bot 2 — left, stacked, face down
        AnimatedCard c2;
        c2.startX = cx; c2.startY = cy;
        c2.targetX = 200.0f + round * 2.0f;
        c2.targetY = 400.0f + round * 2.0f;
        c2.x = cx; c2.y = cy;
        c2.progress = 0.0f;
        c2.done = false;
        c2.faceUp = false;
        c2.card = nullptr;
        animCards.push_back(c2);

        // Bot 3 — right, stacked, face down
        AnimatedCard c3;
        c3.startX = cx; c3.startY = cy;
        c3.targetX = 1000.0f + round * 2.0f;
        c3.targetY = 400.0f + round * 2.0f;
        c3.x = cx; c3.y = cy;
        c3.progress = 0.0f;
        c3.done = false;
        c3.faceUp = false;
        c3.card = nullptr;
        animCards.push_back(c3);

        // Player — bottom spread, face up, uses Card's own texture
        AnimatedCard cp;
        cp.startX = cx; cp.startY = cy;
        cp.targetX = 240.0f + round * 60.0f;
        cp.targetY = 700.0f;
        cp.x = cx; cp.y = cy;
        cp.progress = 0.0f;
        cp.done = false;
        cp.faceUp = true;
        cp.card = &hand[round];
        animCards.push_back(cp);
    }
}

static float easeOut(float t)
{
    return 1.0f - (1.0f - t) * (1.0f - t);
}

void DealAnimation::update(float dt)
{
    if (finished) return;

    dealDelay += dt;
    if (dealDelay >= dealInterval && currentCard < (int)animCards.size())
    {
        dealDelay = 0.0f;
        currentCard++;
    }

    bool allDone = true;
    for (int i = 0; i < currentCard && i < (int)animCards.size(); i++)
    {
        AnimatedCard& c = animCards[i];
        if (!c.done)
        {
            c.progress += dt * 3.0f;
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
        if (c.faceUp && c.card != nullptr)
        {
            float scale = 50.0f / c.card->texture.width;
            DrawTextureEx(
                c.card->texture,
                {c.x - 25.0f, c.y - (c.card->texture.height * scale) / 2.0f},
                0.0f, scale, WHITE);
        }
        else
        {
            r.drawCardBack(c.x, c.y);
        }
    }
}

bool DealAnimation::isDone() const
{
    return finished;
}

int DealAnimation::getDealtCount() const
{
    int count = 0;
    // every 4th animCard is a player card (bot1, bot2, bot3, player, bot1...)
    for (int i = 3; i < currentCard && i < (int)animCards.size(); i += 4)
    {
        if (animCards[i].done)
            count++;
    }
    return count;
}