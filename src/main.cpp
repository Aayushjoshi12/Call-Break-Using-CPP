#include <iostream>
#include <raylib.h>
using namespace std;
int main(){
    int sh=800;
    int sw=1200;
    InitWindow(sw,sh,"Call Break");
    while(!WindowShouldClose()){
        BeginDrawing();
          ClearBackground(RAYWHITE);
          
        EndDrawing();   
    }
    CloseWindow();
    return 0;
}