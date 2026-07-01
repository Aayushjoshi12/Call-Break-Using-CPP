#pragma once
#include "renderer.h"
#include "entities.h"
#include <vector>
#include <string>
class TimeManager
{
public:
    float totalTime = 60.0f;
    float currentTime = 60.0f;

    void update();
    void reset();
};
