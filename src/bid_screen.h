#ifndef BID_SCREEN_H
#define BID_SCREEN_H

#include <raylib.h>

class BidScreen
{
private:

    Texture2D backgroundTexture;

    static const int TOTAL_BID_BUTTONS = 8;

    Rectangle bidButtonRectangle[TOTAL_BID_BUTTONS];

    Rectangle confirmButtonRectangle;

    int selectedBidNumber;

    bool bidConfirmed;

    bool showWarningMessage;

    const char* warningMessage;

public:

    BidScreen();

    ~BidScreen();

    void Update();

    void Draw();

    bool IsBidConfirmed() const;

    int GetSelectedBid() const;

private:

    void LoadResources();

    void UnloadResources();

    void CreateLayout();

    void HandleBidSelection();

    void HandleConfirmButton();

    void DrawBackground();

    void DrawTitle();

    void DrawInstructionText();

    void DrawBidButtons();

    void DrawSelectedBidPanel();

    void DrawConfirmButton();

    void DrawWarningMessage();

    bool IsMouseOverButton(Rectangle buttonRectangle);
};

#endif