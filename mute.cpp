#include "mute.hpp"

mute::mute(const char *imagepathoff,const char *imagepathon, Vector2 imagePosition,float size)
{
    Image image_off = LoadImage(imagepathoff);
    int origialwidth =image_off.width;
    int originalheight = image_off.height;
    int newwidth = static_cast<int>(origialwidth * size);
    int newheight = static_cast<int>(originalheight * size);
        ImageResize(&image_off, newwidth, newheight); 
    pos2 = imagePosition;
     text_off =LoadTextureFromImage(image_off);

    buttonReact = {pos2.x,pos2.y,(float)text_off.width,(float)text_off.height};
     
    UnloadImage(image_off);

   
    Image image_on = LoadImage(imagepathon);
    int origialwidth_on =image_on.width;
    int originalheight_on = image_on.height;
    int newwidth_on = static_cast<int>(origialwidth_on * size);
    int newheight_on = static_cast<int>(originalheight_on * size);
        ImageResize(&image_on, newwidth_on, newheight_on); 
    text_on =LoadTextureFromImage(image_on);
     buttonReacton = {pos2.x,pos2.y,(float)text_on.width,(float)text_on.height};
    
}
mute::~mute()
{
    UnloadTexture(text_off); 
    UnloadTexture(text_on);
}
void mute::drawbutton(){
     if (CheckCollisionPointRec(GetMousePosition(),buttonReact) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
           ismuted = !ismuted;
        }
      
     drawm(); 
}
void mute::drawm(){
          if (ismuted)
{
    DrawTexture(text_off, pos2.x,pos2.y, WHITE);
}
else
{
    DrawTexture(text_on,pos2.x,pos2.y, YELLOW);
}
}