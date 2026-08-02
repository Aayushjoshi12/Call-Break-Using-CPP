
#include <iostream>
#include"../headerfiles/TimeManager.h"
void TimeManager::update(float deltaTime)
{
    currentTime -= deltaTime;
    if (currentTime <= 0)
    {
        currentTime = 0;
    }
}
 
void TimeManager::reset()
{
    currentTime = totalTime;
}