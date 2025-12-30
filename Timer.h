#pragma once

#include <chrono>

class Timer {
private:
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
    Timer() = default;

    void reset() {
        m_beg = Clock::now();
    }

    double now() const {
        return std::chrono::duration_cast<Second>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    double elapsed() const {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};