#pragma once
#include <chrono>
#include <string>

class Timer {
public:
    Timer(const std::string& name = "");
    ~Timer();

    float elapsedSeconds();

private:
    const std::chrono::steady_clock::time_point start_;
    const std::string                           name_;
};