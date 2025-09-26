#ifndef TIMER_HPP
#define TIMER_HPP

#include "./cpu.hpp"
#include <chrono>

enum {
    ms500 = 0,
    ms1000 = 1, 
    ms1500 = 2,
    ms2000 = 3,
    ms5000 = 4,
    ms10000 = 5,
    ms30000 = 6,
    ms60000 = 7
} typedef TIMER_INTERVAL;

class Timer {
public:
    Timer(CPU* cpu) : cpu(cpu), last_tick() { last_tick = std::chrono::system_clock::now(); }
    ~Timer() {}

    void tick();

private:
    CPU* cpu;
    std::chrono::time_point<std::chrono::system_clock> last_tick;

    uint32 configToMS(TIMER_INTERVAL interval);
};

#endif