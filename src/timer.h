#pragma once

class Timer {
public:
    Timer(int period);

    Timer(int period, bool first);

    /// @brief  Resets timer
    void reset();

    /// @brief Checks if the timer is fired
    bool timeout();

private:
    unsigned long lt;
    int period;

    bool first = false;
    bool first_check = true;
};