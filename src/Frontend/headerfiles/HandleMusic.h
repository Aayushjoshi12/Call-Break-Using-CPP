#pragma once
#include "raylib.h"

class HandleMusic {
public:
    HandleMusic();
    ~HandleMusic();

    void Update();
    void Draw();

private:
    Music     m_music;
    Texture2D m_muteBtn;
    Texture2D m_unmuteBtn;
    bool      m_isMuted;
};