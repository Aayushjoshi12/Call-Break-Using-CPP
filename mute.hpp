#include<raylib.h>
class mute{
public:
mute(const char *imagepathoff,const char *imagepathon, Vector2 imagePosition,float size);
~mute();
void drawbutton();
void drawm();
private:
bool ismuted =false;

Texture2D text_off;
    Texture2D text_on;
Vector2 pos2;
 Rectangle buttonReact;
  Rectangle buttonReacton;
};