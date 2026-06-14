#include <raylib.h>
#include "human.hpp"

int main()
{
    InitWindow(1200, 800, "Call break");
    SetTargetFPS(60);

    callall();

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(WHITE);
            callfordrawing();
        EndDrawing();
    }

    unloadall();
    CloseWindow();
    return 0;
}