#include<raylib.h>
class fonta{
    public:
    fonta(const char *fontpath, int fontSize);
    ~fonta();
    void drawf(const char *text, Vector2 position, float fontSize, float spacing, Color tint);

    private:
    Font font;
        Vector2 posa;
};