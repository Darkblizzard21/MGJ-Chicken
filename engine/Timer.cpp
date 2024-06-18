#include "Timer.h"

#include <iostream>

Timer::Timer(const std::string& name) : name_(name), start_(std::chrono::steady_clock::now()) {}

Timer::~Timer()
{
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if (name_.empty()) {
        std::cout << "Nameless Timer Duration: ";
    } else {
        std::cout << "Timer \"" << name_ << "\" Duration : ";
    }

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
    if (duration < 1000) {
        std::cout << duration << " [ns]" << std::endl;
    } else if (duration < 1000 * 1000) {
        float durationF = duration / 1000.f;
        std::cout << durationF << " [µs]" << std::endl;
    } else if (duration < 1000 * 1000 * 1000) {
        auto  durationI = duration / 1000;
        float durationF = durationI / 1000.f;
        std::cout << durationF << " [ms]" << std::endl;
    } else {
        auto  durationI = duration / (1000 * 1000);
        float durationF = durationI / 1000.f;
        std::cout << durationF << " [s]" << std::endl;
    }
}

float Timer::elapsedSeconds()
{
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
    return duration / 1000.f;
}
