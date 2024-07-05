//
// Created by Let'sBlend on 01/07/2024.
//

#ifndef XPLORE_TIMER_H
#define XPLORE_TIMER_H


class Timer
{
public:
    static void StartTime();
    static std::chrono::high_resolution_clock::duration EndTime();

private:
    inline static std::chrono::high_resolution_clock::time_point startTime;
    inline static std::chrono::high_resolution_clock::time_point endTime;
};


#endif //XPLORE_TIMER_H
