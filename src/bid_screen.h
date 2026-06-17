#ifndef BID_SCREEN_H
#define BID_SCREEN_H

#include <raylib.h>

class BidScreen
{
private:
    int selectedBid;
    Rectangle bidButtons[8];

public:
    BidScreen();

    void Update();
    void Draw(Texture2D backgroundImage);

    int GetSelectedBid();
};

#endif