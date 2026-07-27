#pragma once

#include "raylib.h"
#include <string>

class GameLobby
{
public:
    GameLobby();
    ~GameLobby();

    void Draw(int connectedPlayers);

private:
    void DrawTitle();
    void DrawPlayers(int connectedPlayers);
    void DrawBottomText(int connectedPlayers);

    Texture2D background;
    Texture2D avatar;

    Font font;

    Color gold;

    Rectangle playerBox[4];
    Vector2 avatarPos[4];
};