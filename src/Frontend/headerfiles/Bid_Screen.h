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
    bool confirmed;

    void Update();
    void Draw();
    // NEW: selectedBid was never cleared between hands, so a leftover
    // selection (plus the CONFIRM button sitting at the same fixed screen
    // position every hand) let bidding get skipped almost instantly.
    void Reset();

    int GetSelectedBid();
};

#endif