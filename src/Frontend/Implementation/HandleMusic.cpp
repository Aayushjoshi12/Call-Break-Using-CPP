#include "../headerfiles/HandleMusic.h"
#include"../text.h"

static Rectangle Mute_Button = { (float)1100.0, (float)70.0, (float)45.0, (float)45.0 };

HandleMusic::HandleMusic()
    : m_music({}), m_muteBtn({}), m_unmuteBtn({}), m_isMuted(false)
{
    m_music     = LoadMusicStream(MUSIC_CASINO);
    m_muteBtn   = LoadTexture(IMG_MUTE);
    m_unmuteBtn = LoadTexture(IMG_UNMUTE);
    PlayMusicStream(m_music);
}

HandleMusic::~HandleMusic()
{
    UnloadMusicStream(m_music);
    UnloadTexture(m_muteBtn);
    UnloadTexture(m_unmuteBtn);
}

void HandleMusic::Update()
{
    UpdateMusicStream(m_music);

    if (CheckCollisionPointRec(GetMousePosition(), Mute_Button) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        m_isMuted = !m_isMuted;
        SetMusicVolume(m_music, m_isMuted ? 0.0f : 1.0f);
    }
}

void HandleMusic::Draw()
{
    Texture2D& btn = m_isMuted ? m_muteBtn : m_unmuteBtn;

    DrawTexturePro(
        btn,
        (Rectangle){ 0, 0, (float)btn.width, (float)btn.height },
        Mute_Button,
        (Vector2){ 0, 0 },
        0.0f,
        WHITE);
}