#ifndef BID_SCREEN_H
#define BID_SCREEN_H

#include <raylib.h>

class BidScreen
{
private:
    int selectedBid;
    Font cinzel;
    Rectangle bidButtons[8];
    Texture2D backgroundImage;

public:
    BidScreen();

    void Update();
    void Draw();

    int GetSelectedBid();
};

#endif