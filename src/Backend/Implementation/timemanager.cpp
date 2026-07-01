#include "gamemanager.h"
#include <iostream>
void TimeManager::update()
{
    currentTime -= GetFrameTime();
    if (currentTime <= 0)
    {
        currentTime = 0;
    }
}

void TimeManager::reset()
{
    currentTime = totalTime;
}
