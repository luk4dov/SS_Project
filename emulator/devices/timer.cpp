#include "./timer.hpp"

void Timer::tick() {

    auto now = std::chrono::system_clock::now();
    uint32 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();

    TIMER_INTERVAL interval = static_cast<TIMER_INTERVAL>(cpu->readMem(cpu->getTimCfg()));

    if(elapsed > configToMS(interval)) {
        last_tick = now;
        cpu->interrupt(INT_CAUSE::TIMER);
    }
}

uint32 Timer::configToMS(TIMER_INTERVAL interval) {
    switch(interval) {
        case ms500: return 500;
        case ms1000: return 1000;
        case ms1500: return 1500;
        case ms2000: return 2000;
        case ms5000: return 5000;
        case ms10000: return 10000;
        case ms30000: return 30000;
        case ms60000: return 60000;
        default: return 500;
    }
}
