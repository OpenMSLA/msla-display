#include "timer.h"

#include <Arduino.h>

Timer::Timer(int t) {
    period = t;
}

Timer::Timer(int t, bool first) {
    period = t;
    first = first;
}

void Timer::reset() {
    lt = millis();
    first_check = true;
}

bool Timer::timeout() {
    if (millis() - lt > period || (first && first_check)) {
        lt = millis();
        first_check = false;
        return true;
    }

    return false;
}