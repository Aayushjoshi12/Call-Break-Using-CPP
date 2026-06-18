#pragma once
#include "raylib.h"
#include <vector>
#include <string>
 
// ──────────────────────────────────────────────────────────────
//  Constants
// ──────────────────────────────────────────────────────────────
static constexpr int   SW         = 1280;
static constexpr int   SH         = 800;
static constexpr int   NUM_CARDS  = 52;
static constexpr float CARD_SCALE = 0.20f;
static constexpr int   FPS        = 60;
 
// ──────────────────────────────────────────────────────────────
//  Enums
// ──────────────────────────────────────────────────────────────
enum class Phase {
    IDLE,
    CUT_LIFT,
    CUT_DROP,
    CUT_MERGE,
    RIFFLE_SPLIT,
    RIFFLE_DROP,
    RIFFLE_PUSH,
    OVERHAND,
    SQUARE,
};
 
// ──────────────────────────────────────────────────────────────
//  Structs
// ──────────────────────────────────────────────────────────────
struct Card {
    Vector2 pos;
    float   rot;
    float   scale;
 
    Vector2 posA, posB;
    float   rotA, rotB;
    float   scaleA, scaleB;
 
    float   delay;
    float   dur;
    float   arcH;
    int     zOrder;
    int     half;
    float   seed;
    bool    landed;
};
 
struct OHPacket {
    int     start, count;
    float   startT;
    float   dur;
    Vector2 srcPos;
    Vector2 dstPos;
    float   dstRot;
    float   arcH;
};
 
// ──────────────────────────────────────────────────────────────
//  CardShuffle Class
// ──────────────────────────────────────────────────────────────
class CardShuffle {
public:
    CardShuffle();
    ~CardShuffle();
 
    // Core lifecycle
    void Init(const char* imagePath);
    void Update();
    void Draw();
    void Unload();
 
    // Run the full loop
    void Run(const char* imagePath);
 
private:
    // ── Card data ──
    std::vector<Card>     m_cards;
    std::vector<int>      m_dropOrder;
    std::vector<OHPacket> m_packets;
 
    // ── Phase state ──
    Phase       m_phase;
    float       m_time;
    float       m_globalTime;
    float       m_duration;
    bool        m_setupDone;
    const char* m_label;
 
    // ── Cut state ──
    int         m_cutAt;
 
    // ── Riffle state ──
    int         m_riffleCount;
    int         m_rifflesTotal;
    Vector2     m_leftBase;
    Vector2     m_rightBase;
 
    // ── Overhand state ──
    int         m_overhands;
 
    // ── Rendering ──
    Texture2D   m_tex;
    Texture2D   m_bgTex;
    float       m_cardW;
    float       m_cardH;
    bool        m_placeholder;
 
    // ── Font & animated text ──
    Font        m_font;       // Cinzel-Bold loaded via LoadFontEx
    float       m_dotTimer;   // counts seconds between dot updates
    int         m_dotCount;   // 0-3: how many dots to show after the label
 
    // ── Math helpers ──
    float   Lerp(float a, float b, float t);
    Vector2 LerpV(Vector2 a, Vector2 b, float t);
    float   Clamp01(float t);
    float   Smooth(float t);
    float   EaseOut(float t);
    float   EaseIn(float t);
    float   SpringSettle(float t, float bounce = 0.28f);
    float   RandF(float lo, float hi);
    int     RandI(int lo, int hi);
    float   SmoothNoise(float x);
 
    // ── Card helpers ──
    void SnapSource(Card& c);
    void StackAllCards(float baseRot = 0.f);
    void AnimateCard(Card& c, float phaseT, Phase ph);
 
    // ── Phase setups ──
    void SetupIdle();
    void SetupCutLift();
    void SetupCutDrop();
    void SetupCutMerge();
    void SetupRiffleSplit();
    void SetupRiffleDrop();
    void SetupRifflePush();
    void SetupOverhand();
    void SetupSquare();
 
    // ── Phase update ──
    void UpdateOverhand(float elapsed);
    void ApplyTremor();
    void StartPhase(Phase p);
    void AdvancePhase();
    void RunSetup();
 
    // ── Draw helpers ──
    void DrawCards();
    void DrawUI();
    void LoadCardTexture(const char* imagePath);
};