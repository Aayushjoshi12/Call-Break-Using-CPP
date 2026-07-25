#include "../headerfiles/lobby.h"

GameLobby::GameLobby()
{
    background = LoadTexture("../Assets/Image files/BackGround.jpg");

    avatar = LoadTexture("../Assets/Image files/human.png");

    font = LoadFontEx("../Assets/fonts/Cinzel-Bold.ttf", 96, 0, 0);

    gold = {212, 175, 55, 255};

    playerBox[0] = {70, 110, 200, 200};
    playerBox[1] = {70, 480, 200, 200};
    playerBox[2] = {880, 110, 200, 200};
    playerBox[3] = {880, 480, 200, 200};

    for (int i = 0; i < 4; i++)
    {
        avatarPos[i].x = playerBox[i].x ;
        avatarPos[i].y = playerBox[i].y ;
    }
}

GameLobby::~GameLobby()
{
    UnloadTexture(background);
    UnloadTexture(avatar);

    UnloadFont(font);
}

void GameLobby::Draw(int connectedPlayers)
{
    DrawTexturePro(
        background,
        {0,
         0,
         (float)background.width,
         (float)background.height},
        {0,
         0,
         (float)GetScreenWidth(),
         (float)GetScreenHeight()},
        {0, 0},
        0,
        WHITE);

    DrawTitle();

    DrawPlayers(connectedPlayers);

    DrawBottomText(connectedPlayers);
}

void GameLobby::DrawTitle()
{
    const char *title = "GAME LOBBY";

    float fontSize = 70;

    Vector2 size = MeasureTextEx(font, title, fontSize, 2);

    DrawTextEx(
        font,
        title,
        {(GetScreenWidth() - size.x) / 2,
         20},
        fontSize,
        2,
        gold);
}

void GameLobby::DrawPlayers(int connectedPlayers)
{
    for (int i = 0; i < 4; i++)
    {
        bool connected = (i < connectedPlayers);

        Color border = connected ? WHITE : Fade(GRAY, 0.45f);
        Color tint = connected ? WHITE : Fade(GRAY, 0.35f);

        DrawRectangleRounded(
            playerBox[i],
            0.18f,
            20,
            BLACK);

        DrawRectangleRoundedLines(
            playerBox[i],
            0.18f,
            20,
            border);

        DrawTextureEx(
            avatar,
            avatarPos[i],
            0,
            0.60f,
            tint);

        std::string txt = "PLAYER " + std::to_string(i + 1);

        Vector2 t = MeasureTextEx(
            font,
            txt.c_str(),
            28,
            2);

        DrawTextEx(
            font,
            txt.c_str(),
            {playerBox[i].x + playerBox[i].width / 2 - t.x / 2,
             playerBox[i].y + playerBox[i].height + 18},
            28,
            2,
            WHITE);
    }
}

void GameLobby::DrawBottomText(int connectedPlayers)
{
    std::string status =
        std::to_string(connectedPlayers) + "/4 HAVE COME";

    Vector2 s = MeasureTextEx(
        font,
        status.c_str(),
        30,
        2);

    DrawTextEx(
        font,
        status.c_str(),
        {(GetScreenWidth() - s.x) / 2,
         640},
        30,
        2,
        WHITE);

    const char *waiting =
        "WAITING FOR REMAINING PLAYERS......";

    Vector2 w = MeasureTextEx(
        font,
        waiting,
        46,
        2);

    DrawTextEx(
        font,
        waiting,
        {(GetScreenWidth() - w.x) / 2,
         720},
        46,
        2,
        gold);
}