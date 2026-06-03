#include <raylib.h>

int main()
{
    InitWindow(1200, 800, "Call Break Bid");

    Texture2D bg = LoadTexture("Assets/background.png");

    int selectedBid = 0;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        Rectangle buttons[8] =
        {
            {180,250,100,100},
            {380,250,100,100},
            {580,250,100,100},
            {780,250,100,100},

            {180,450,100,100},
            {380,450,100,100},
            {580,450,100,100},
            {780,450,100,100}
        };

        for(int i=0;i<8;i++)
        {
            if(CheckCollisionPointRec(mouse,buttons[i]) &&
               IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selectedBid = i+1;
            }
        }

        BeginDrawing();

        DrawTexture(bg,0,0,WHITE);

        DrawText("ENTER YOUR BID",400,80,50,GOLD);

        for(int i=0;i<8;i++)
        {
            Color c = LIGHTGRAY;

            if(selectedBid==i+1)
                c = YELLOW;

            DrawCircle(buttons[i].x+50,
                       buttons[i].y+50,
                       50,
                       c);

            DrawText(TextFormat("%d",i+1),
                     buttons[i].x+35,
                     buttons[i].y+25,
                     40,
                     BLACK);
        }

        DrawText(
            TextFormat("Selected Bid: %d",selectedBid),
            430,
            700,
            35,
            GREEN
        );

        EndDrawing();
    }

    UnloadTexture(bg);
    CloseWindow();
}