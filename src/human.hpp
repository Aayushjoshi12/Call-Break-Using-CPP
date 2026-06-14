#pragma once
#include <raylib.h>

// ─── Man ─────────────────────────────────────────────────────────────────────
class Man {
public:
    Texture2D texture;
    Vector2   pos;

    void load(const char *imagepath, Vector2 position, float size);
    void draw(Color tint = WHITE);
    void drawBox(Color borderColor, float thickness = 3.0f);
    void unload();
};

// ─── Mute ────────────────────────────────────────────────────────────────────
class Mute {
public:
    Texture2D text_off;
    Texture2D text_on;
    Vector2   pos;
    Rectangle buttonRect;
    bool      ismuted;

    void load(const char *pathOff, const char *pathOn,
              Vector2 position, float size);
    void drawbutton();   // handles click + draws
    void unload();
};

// ─── Fonta ───────────────────────────────────────────────────────────────────
class Fonta {
public:
    Font font;

    void load(const char *fontpath, int fontSize);
    void drawf(const char *text, Vector2 position,
               float fontSize, float spacing, Color tint);
    void unload();
};

// ─── Game-level functions ────────────────────────────────────────────────────
void callall();
void callfordrawing();
void unloadall();