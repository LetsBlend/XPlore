//
// Created by Let'sBlend on 01/07/2024.
//

#include "Timer.h"
#include "Debugger.h"

void Timer::StartTime()
{
    startTime = std::chrono::high_resolution_clock::now();
}

std::chrono::high_resolution_clock::duration Timer::EndTime()
{
    endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
}
