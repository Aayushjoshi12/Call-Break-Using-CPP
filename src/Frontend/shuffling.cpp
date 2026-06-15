#include "shuffling.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

static const Vector2 TABLE = { SW * 0.5f, SH * 0.52f };

// ──────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ──────────────────────────────────────────────────────────────
CardShuffle::CardShuffle()
    : m_phase(Phase::IDLE), m_time(0.f), m_globalTime(0.f),
      m_duration(1.f), m_setupDone(false), m_label(""),
      m_cutAt(0), m_riffleCount(0), m_rifflesTotal(2),
      m_leftBase({0,0}), m_rightBase({0,0}),
      m_overhands(0), m_tex({0}), m_cardW(0), m_cardH(0),
      m_placeholder(false)
{
    srand((unsigned)time(nullptr));
}

CardShuffle::~CardShuffle() {}

// ──────────────────────────────────────────────────────────────
//  Math Helpers
// ──────────────────────────────────────────────────────────────
float CardShuffle::Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

Vector2 CardShuffle::LerpV(Vector2 a, Vector2 b, float t) {
    return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
}

float CardShuffle::Clamp01(float t) {
    return t < 0.f ? 0.f : t > 1.f ? 1.f : t;
}

float CardShuffle::Smooth(float t) {
    t = Clamp01(t);
    return t * t * (3.f - 2.f * t);
}

float CardShuffle::EaseOut(float t) {
    t = Clamp01(t);
    return 1.f - (1.f - t) * (1.f - t) * (1.f - t);
}

float CardShuffle::EaseIn(float t) {
    t = Clamp01(t);
    return t * t * t;
}

float CardShuffle::SpringSettle(float t, float bounce) {
    t = Clamp01(t);
    return 1.f - expf(-6.f * t) * cosf(2.f * PI * bounce * t * 3.f);
}

float CardShuffle::RandF(float lo, float hi) {
    return lo + (hi - lo) * (rand() / (float)RAND_MAX);
}

int CardShuffle::RandI(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

float CardShuffle::SmoothNoise(float x) {
    int   i = (int)floorf(x);
    float f = x - (float)i;
    float u = f * f * (3.f - 2.f * f);
    auto  h = [](int n) -> float {
        n = (n << 13) ^ n;
        return 1.f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.f;
    };
    return Lerp(h(i), h(i + 1), u);
}

// ──────────────────────────────────────────────────────────────
//  Card Helpers
// ──────────────────────────────────────────────────────────────
void CardShuffle::SnapSource(Card& c) {
    c.posA   = c.pos;
    c.rotA   = c.rot;
    c.scaleA = c.scale;
    c.delay  = 0.f;
    c.dur    = 1.f;
    c.arcH   = 0.f;
    c.landed = false;
}

void CardShuffle::StackAllCards(float baseRot) {
    for (int i = 0; i < NUM_CARDS; ++i) {
        m_cards[i].pos   = { TABLE.x + RandF(-1.2f, 1.2f),
                              TABLE.y - i * 0.28f + RandF(-0.4f, 0.4f) };
        m_cards[i].rot   = baseRot + RandF(-0.7f, 0.7f);
        m_cards[i].scale = 1.f;
        m_cards[i].zOrder = i;
    }
}

void CardShuffle::AnimateCard(Card& c, float phaseT, Phase ph) {
    if (c.landed) return;
    float lt = (phaseT - c.delay) / fmaxf(c.dur - c.delay, 0.001f);
    lt = Clamp01(lt);
    if (lt <= 0.f) return;

    float e;
    switch (ph) {
        case Phase::CUT_LIFT:    e = EaseOut(lt);             break;
        case Phase::CUT_DROP:    e = SpringSettle(lt, 0.15f); break;
        case Phase::RIFFLE_DROP: e = EaseOut(lt);             break;
        case Phase::SQUARE:      e = SpringSettle(lt, 0.18f); break;
        default:                 e = Smooth(lt);              break;
    }

    c.pos   = LerpV(c.posA, c.posB, e);
    c.rot   = Lerp(c.rotA, c.rotB, e);
    c.scale = Lerp(c.scaleA, c.scaleB, e);

    if (c.arcH != 0.f)
        c.pos.y -= sinf(lt * PI) * c.arcH;

    if (lt >= 1.f) c.landed = true;
}

// ──────────────────────────────────────────────────────────────
//  Phase Setups
// ──────────────────────────────────────────────────────────────
void CardShuffle::SetupIdle() {
    m_label    = "Deck at rest";
    m_duration = 1.4f;
    for (auto& c : m_cards) {
        SnapSource(c);
        c.posB   = c.pos;
        c.rotB   = c.rot;
        c.scaleB = 1.f;
    }
}

void CardShuffle::SetupCutLift() {
    m_label    = "Cutting...";
    m_duration = 0.56f;
    m_cutAt    = RandI(16, 26);

    float cx = TABLE.x + 165.f + RandF(-8.f, 8.f);
    float cy = TABLE.y - RandF(4.f, 12.f);

    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c  = m_cards[i];
        SnapSource(c);
        bool  top = (i >= NUM_CARDS - m_cutAt);
        if (top) {
            int   local = i - (NUM_CARDS - m_cutAt);
            float frac  = (float)local / m_cutAt;
            c.posB  = { cx + RandF(-2.f, 2.f),
                        cy - local * 0.28f + RandF(-0.5f, 0.5f) };
            c.rotB  = RandF(-2.5f, 2.5f);
            c.arcH  = 38.f + frac * 15.f;
            c.delay = powf(frac, 0.8f) * 0.18f + RandF(-0.005f, 0.01f);
            c.dur   = 0.65f + frac * 0.1f + RandF(-0.03f, 0.03f);
        } else {
            c.posB = c.pos;
            c.rotB = c.rot;
        }
        c.scaleB = 1.f;
    }
}

void CardShuffle::SetupCutDrop() {
    m_label    = "Placing cut...";
    m_duration = 0.42f;
    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c  = m_cards[i];
        SnapSource(c);
        bool  top = (i >= NUM_CARDS - m_cutAt);
        c.posB  = { c.pos.x + RandF(-1.f, 1.f),
                    c.pos.y + (top ? RandF(3.f, 8.f) : 0.f) };
        c.rotB  = c.rot + (top ? RandF(-0.5f, 0.5f) : 0.f);
        c.scaleB = 1.f;
        c.delay  = 0.f;
        c.dur    = 0.55f + RandF(-0.03f, 0.03f);
    }
}

void CardShuffle::SetupCutMerge() {
    m_label    = "Completing cut";
    m_duration = 0.62f;
    float botTop = TABLE.y - (float)(NUM_CARDS - m_cutAt) * 0.28f;

    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c  = m_cards[i];
        SnapSource(c);
        bool  top = (i >= NUM_CARDS - m_cutAt);
        if (top) {
            int   local = i - (NUM_CARDS - m_cutAt);
            float frac  = (float)local / m_cutAt;
            c.posB  = { TABLE.x + RandF(-1.5f, 1.5f),
                        botTop - local * 0.28f };
            c.rotB  = RandF(-0.7f, 0.7f);
            c.arcH  = 20.f + local * 0.3f;
            c.delay = powf(frac, 0.7f) * 0.22f + RandF(-0.01f, 0.02f);
            c.dur   = 0.75f;
        } else {
            c.posB = c.pos;
            c.rotB = c.rot;
        }
        c.scaleB = 1.f;
    }
}

void CardShuffle::SetupRiffleSplit() {
    m_label    = "Splitting for riffle...";
    m_duration = 0.62f;
    int half   = NUM_CARDS / 2;

    m_leftBase  = { TABLE.x - 90.f + RandF(-4.f, 4.f), TABLE.y + 8.f + RandF(-3.f, 3.f) };
    m_rightBase = { TABLE.x + 90.f + RandF(-4.f, 4.f), TABLE.y + 8.f + RandF(-3.f, 3.f) };

    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c    = m_cards[i];
        SnapSource(c);
        bool  left  = (i < half);
        int   local = left ? i : (i - half);
        float frac  = (float)local / (half - 1);
        float side  = left ? -1.f : 1.f;
        Vector2 base = left ? m_leftBase : m_rightBase;

        c.posB  = { base.x + side * frac * 16.f + RandF(-2.f, 2.f),
                    base.y - local * 0.27f + RandF(-0.8f, 0.8f) };
        c.rotB  = side * (1.5f + frac * 4.f) * -1.f + RandF(-0.6f, 0.6f);
        c.arcH  = frac * 12.f;
        c.delay = 0.04f + frac * 0.15f + RandF(-0.01f, 0.02f);
        c.dur   = 0.70f;
        c.scaleB = 1.f;
        c.half  = left ? 0 : 1;
        c.zOrder = i;
    }
}

void CardShuffle::SetupRiffleDrop() {
    m_label    = "Riffling...";
    m_duration = 2.6f;

    std::vector<int> leftIdx, rightIdx;
    for (int i = 0; i < NUM_CARDS; ++i) {
        if (m_cards[i].half == 0) leftIdx.push_back(i);
        else                       rightIdx.push_back(i);
    }
    std::reverse(leftIdx.begin(), leftIdx.end());
    std::reverse(rightIdx.begin(), rightIdx.end());

    m_dropOrder.clear();
    int li = 0, ri = 0;
    bool pickLeft = (rand() % 2 == 0);
    while (li < (int)leftIdx.size() || ri < (int)rightIdx.size()) {
        int burst = (RandF(0, 1) < 0.6f) ? 1 : (RandF(0, 1) < 0.7f) ? 2 : 3;
        if (pickLeft) {
            for (int b = 0; b < burst && li < (int)leftIdx.size(); ++b, ++li)
                m_dropOrder.push_back(leftIdx[li]);
        } else {
            for (int b = 0; b < burst && ri < (int)rightIdx.size(); ++b, ++ri)
                m_dropOrder.push_back(rightIdx[ri]);
        }
        pickLeft = !pickLeft;
    }

    int total = (int)m_dropOrder.size();
    for (int slot = 0; slot < total; ++slot) {
        Card& c = m_cards[m_dropOrder[slot]];
        SnapSource(c);
        float side = (c.half == 0) ? -1.f : 1.f;
        c.posB  = { TABLE.x + RandF(-5.f, 5.f) + side * RandF(0.f, 6.f),
                    TABLE.y - slot * 0.28f + RandF(-1.f, 1.f) };
        c.rotB  = RandF(-1.8f, 1.8f);
        c.scaleB = 1.f;
        c.arcH  = 22.f + RandF(-5.f, 8.f);
        c.delay = (float)slot / total * 0.86f + RandF(-0.008f, 0.008f);
        c.dur   = 1.0f;
        c.zOrder = slot;
        c.landed = false;
    }
}

void CardShuffle::SetupRifflePush() {
    m_label    = "Pushing together...";
    m_duration = 0.52f;
    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c = m_cards[i];
        SnapSource(c);
        c.arcH  = 0.f;
        c.posB  = { TABLE.x + RandF(-2.f, 2.f),
                    TABLE.y - i * 0.29f + RandF(-0.8f, 0.8f) };
        c.rotB  = RandF(-0.9f, 0.9f);
        c.scaleB = 1.f;
        c.delay  = 0.f;
        c.dur    = 1.0f + RandF(-0.03f, 0.03f);
        c.zOrder = i;
    }
}

void CardShuffle::SetupOverhand() {
    m_label = "Overhand shuffle...";

    Vector2 topHand = { TABLE.x - 75.f + RandF(-5.f, 5.f),
                        TABLE.y - 55.f  + RandF(-5.f, 5.f) };
    Vector2 botHand = { TABLE.x + 25.f + RandF(-5.f, 5.f), TABLE.y + 2.f };

    m_packets.clear();
    int   remaining = NUM_CARDS;
    int   cardIdx   = NUM_CARDS - 1;
    float t         = 0.f;

    while (remaining > 0) {
        int burst = std::min(RandI(2, 6), remaining);
        OHPacket p;
        p.start  = cardIdx - burst + 1;
        p.count  = burst;
        p.startT = t;
        p.srcPos = { topHand.x + RandF(-3.f, 3.f),
                     topHand.y - (NUM_CARDS - 1 - cardIdx) * 0.27f };
        p.dstPos = { botHand.x + RandF(-12.f, 12.f),
                     botHand.y + (float)(NUM_CARDS - remaining) * 0.27f + RandF(-3.f, 3.f) };
        p.dstRot = RandF(-4.f, 4.f);
        p.arcH   = RandF(30.f, 55.f);
        p.dur    = 0.18f + RandF(0.f, 0.05f);
        m_packets.push_back(p);

        t        += 0.22f + RandF(0.f, 0.1f);
        cardIdx  -= burst;
        remaining -= burst;
    }

    m_duration = t + 0.45f;

    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c = m_cards[i];
        SnapSource(c);
        c.posB  = { topHand.x + RandF(-3.f, 3.f),
                    topHand.y - i * 0.27f };
        c.rotB  = RandF(-5.f, 5.f);
        c.scaleB = 1.f;
        c.zOrder = i;
    }
}

void CardShuffle::SetupSquare() {
    m_label    = "Squaring deck...";
    m_duration = 0.75f;
    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c = m_cards[i];
        SnapSource(c);
        c.arcH  = 0.f;
        c.posB  = { TABLE.x + RandF(-0.6f, 0.6f),
                    TABLE.y - i * 0.29f + RandF(-0.2f, 0.2f) };
        c.rotB  = RandF(-0.3f, 0.3f);
        c.scaleB = 1.f;
        c.delay  = 0.f;
        c.dur    = 1.0f;
        c.zOrder = i;
    }
}

// ──────────────────────────────────────────────────────────────
//  Phase Updates
// ──────────────────────────────────────────────────────────────
void CardShuffle::UpdateOverhand(float elapsed) {
    for (const auto& p : m_packets) {
        float lt = Clamp01((elapsed - p.startT) / p.dur);
        if (lt <= 0.f) continue;
        float e   = EaseOut(lt);
        float arc = -sinf(lt * PI) * p.arcH;
        float srcR = m_cards[p.start].rotA;
        for (int ci = p.start; ci < p.start + p.count && ci < NUM_CARDS; ++ci) {
            Card& c = m_cards[ci];
            c.pos   = LerpV(p.srcPos, p.dstPos, e);
            c.pos.y += arc * (1.f - e);
            c.rot   = Lerp(srcR, p.dstRot, e);
            c.scale = 1.f;
            c.zOrder = (int)(elapsed * 80.f) + ci;
        }
    }
}

void CardShuffle::ApplyTremor() {
    for (auto& c : m_cards) {
        float dx = SmoothNoise((m_globalTime + c.seed) * 1.3f)        * 0.35f;
        float dy = SmoothNoise((m_globalTime + c.seed + 50) * 1.7f)   * 0.18f;
        float dr = SmoothNoise((m_globalTime + c.seed + 25) * 0.8f)   * 0.08f;
        c.pos.x += dx;
        c.pos.y += dy;
        c.rot   += dr;
    }
}

void CardShuffle::StartPhase(Phase p) {
    m_phase     = p;
    m_time      = 0.f;
    m_setupDone = false;
}

void CardShuffle::AdvancePhase() {
    for (auto& c : m_cards) {
        if (m_phase != Phase::OVERHAND && m_phase != Phase::RIFFLE_DROP) {
            c.pos   = c.posB;
            c.rot   = c.rotB;
            c.scale = c.scaleB;
        }
    }
    switch (m_phase) {
        case Phase::IDLE:         StartPhase(Phase::CUT_LIFT);    break;
        case Phase::CUT_LIFT:     StartPhase(Phase::CUT_DROP);    break;
        case Phase::CUT_DROP:     StartPhase(Phase::CUT_MERGE);   break;
        case Phase::CUT_MERGE:
            m_riffleCount  = 0;
            m_rifflesTotal = RandI(2, 3);
            StartPhase(Phase::RIFFLE_SPLIT);
            break;
        case Phase::RIFFLE_SPLIT: StartPhase(Phase::RIFFLE_DROP); break;
        case Phase::RIFFLE_DROP:  StartPhase(Phase::RIFFLE_PUSH); break;
        case Phase::RIFFLE_PUSH:
            m_riffleCount++;
            if (m_riffleCount < m_rifflesTotal)
                StartPhase(Phase::RIFFLE_SPLIT);
            else { m_overhands = 0; StartPhase(Phase::OVERHAND); }
            break;
        case Phase::OVERHAND:
            m_overhands++;
            if (m_overhands < 2) StartPhase(Phase::OVERHAND);
            else                  StartPhase(Phase::SQUARE);
            break;
        case Phase::SQUARE: StartPhase(Phase::IDLE); break;
    }
}

void CardShuffle::RunSetup() {
    if (m_setupDone) return;
    switch (m_phase) {
        case Phase::IDLE:         SetupIdle();        break;
        case Phase::CUT_LIFT:     SetupCutLift();     break;
        case Phase::CUT_DROP:     SetupCutDrop();     break;
        case Phase::CUT_MERGE:    SetupCutMerge();    break;
        case Phase::RIFFLE_SPLIT: SetupRiffleSplit(); break;
        case Phase::RIFFLE_DROP:  SetupRiffleDrop();  break;
        case Phase::RIFFLE_PUSH:  SetupRifflePush();  break;
        case Phase::OVERHAND:     SetupOverhand();    break;
        case Phase::SQUARE:       SetupSquare();      break;
    }
    m_setupDone = true;
}

// ──────────────────────────────────────────────────────────────
//  Draw Helpers
// ──────────────────────────────────────────────────────────────
void CardShuffle::DrawCards() {
    std::vector<int> idx(NUM_CARDS);
    for (int i = 0; i < NUM_CARDS; ++i) idx[i] = i;
    std::stable_sort(idx.begin(), idx.end(),
    [&](int a, int b){ return m_cards[a].zOrder < m_cards[b].zOrder; });

    for (int i : idx) {
        const Card& c = m_cards[i];
        float w = m_cardW * c.scale;
        float h = m_cardH * c.scale;
        Rectangle src  = { 0, 0, (float)m_tex.width, (float)m_tex.height };
        Rectangle dest = { c.pos.x, c.pos.y, w, h };
        Vector2   org  = { w * 0.5f, h * 0.5f };
        Rectangle sdw  = { c.pos.x + 2.f, c.pos.y + 4.f, w, h };
        DrawRectanglePro(sdw, org, c.rot, { 0, 0, 0, 50 });
        DrawTexturePro(m_tex, src, dest, org, c.rot, WHITE);
    }
}

void CardShuffle::DrawUI() {
    // Bottom bar
    DrawRectangle(0, SH - 40, SW, 40, { 0, 0, 0, 110 });
    DrawText(m_label, 16, SH - 28, 18, YELLOW);

    // Progress bar
    int   bx = 260, bw = SW - 360;
    float pT = Clamp01(m_time / m_duration);
    DrawRectangle(bx, SH - 22, bw, 8, { 80, 80, 80, 120 });
    DrawRectangle(bx, SH - 22, (int)(bw * pT), 8, { 220, 200, 80, 200 });

    // FPS
    char buf[32];
    snprintf(buf, sizeof(buf), "FPS: %d", GetFPS());
    DrawText(buf, SW - 90, SH - 28, 18, LIGHTGRAY);

    if (m_placeholder)
        DrawText("(placeholder — image not found)", 16, 12, 16, { 255, 140, 80, 220 });
}

void CardShuffle::LoadCardTexture(const char* imagePath) {
    m_tex = LoadTexture(imagePath);
    m_placeholder = (m_tex.id == 0);
    if (m_placeholder) {
        Image img = GenImageColor(63, 88, { 25, 60, 140, 255 });
        ImageDrawRectangleLines(&img, { 2, 2, 59, 84 }, 2, WHITE);
        ImageDrawRectangleLines(&img, { 5, 5, 53, 78 }, 1, { 180, 200, 255, 255 });
        m_tex = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    SetTextureFilter(m_tex, TEXTURE_FILTER_BILINEAR);
    m_cardW = m_tex.width  * CARD_SCALE;
    m_cardH = m_tex.height * CARD_SCALE;
}

// ──────────────────────────────────────────────────────────────
//  Public Lifecycle
// ──────────────────────────────────────────────────────────────
void CardShuffle::Init(const char* imagePath) {
    LoadCardTexture(imagePath);

    m_cards.resize(NUM_CARDS);
    StackAllCards();
    for (int i = 0; i < NUM_CARDS; ++i) {
        Card& c  = m_cards[i];
        c.posA   = c.pos;  c.posB   = c.pos;
        c.rotA   = c.rot;  c.rotB   = c.rot;
        c.scaleA = 1.f;    c.scaleB = 1.f;
        c.delay  = 0.f;    c.dur    = 1.f;
        c.arcH   = 0.f;
        c.half   = 0;
        c.seed   = RandF(0.f, 100.f);
        c.landed = false;
    }

    m_phase     = Phase::IDLE;
    m_duration  = 1.2f;
    m_setupDone = true;
    m_label     = "Deck at rest";
}

void CardShuffle::Update() {
    float dt     = GetFrameTime();
    m_time       += dt;
    m_globalTime += dt;

    RunSetup();

    float phaseT = Clamp01(m_time / m_duration);

    if (m_phase == Phase::OVERHAND) {
        UpdateOverhand(m_time);
    } else {
        for (auto& c : m_cards)
            AnimateCard(c, phaseT, m_phase);
    }

    if (m_phase == Phase::IDLE || m_phase == Phase::SQUARE)
        ApplyTremor();

    if (m_time >= m_duration) {
        for (auto& c : m_cards) {
            if (m_phase != Phase::OVERHAND && m_phase != Phase::RIFFLE_DROP) {
                c.pos   = c.posB;
                c.rot   = c.rotB;
                c.scale = c.scaleB;
            }
        }
        AdvancePhase();
    }
}

void CardShuffle::Draw() {
    // REMOVED BeginDrawing() and ClearBackground — main handles these

    // Table lines
    for (int y = 0; y < SH; y += 3)
        DrawLine(0, y, SW, y, { 18, 72, 36, 30 });

    // Table shadow ellipse
    DrawEllipse((int)TABLE.x, (int)(TABLE.y + m_cardH * 0.48f),
                (int)(m_cardW * 0.52f), 10, { 0, 0, 0, 55 });

    // Sort and draw cards
    std::vector<int> idx(NUM_CARDS);
    for (int i = 0; i < NUM_CARDS; ++i) idx[i] = i;
    auto& cards = m_cards;
    std::stable_sort(idx.begin(), idx.end(),
        [&cards](int a, int b){ return cards[a].zOrder < cards[b].zOrder; });

    for (int i : idx) {
        const Card& c = m_cards[i];
        float w = m_cardW * c.scale;
        float h = m_cardH * c.scale;
        Rectangle src  = { 0, 0, (float)m_tex.width, (float)m_tex.height };
        Rectangle dest = { c.pos.x, c.pos.y, w, h };
        Vector2   org  = { w * 0.5f, h * 0.5f };
        Rectangle sdw  = { c.pos.x + 2.f, c.pos.y + 4.f, w, h };
        DrawRectanglePro(sdw, org, c.rot, { 0, 0, 0, 50 });
        DrawTexturePro(m_tex, src, dest, org, c.rot, WHITE);
    }

    DrawUI();

    // REMOVED EndDrawing() — main handles this
}
void CardShuffle::Unload() {
    UnloadTexture(m_tex);
}

void CardShuffle::Run(const char* imagePath) {
    InitWindow(SW, SH, "Natural Card Shuffle — Raylib C++");
    SetTargetFPS(FPS);

    Init(imagePath);

    while (!WindowShouldClose()) {
        Update();
        Draw();
    }

    Unload();
    CloseWindow();
}