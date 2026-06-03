#include<raylib.h>
class mute{
public:
mute(const char *imagepath, Vector2 imagePosition,float size);
~mute();
void drawm();
private:
Texture2D text2;
Vector2 pos2;
};