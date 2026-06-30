#pragmam once

#include <raylib.h>

class ResultScreen
{
private:
    Font myFont;
    int scores[5][4];

public:
    ResultScreen();
    ~ResultScreen();

    void SetScore(int round, int player, int score);
    void Draw();
};
