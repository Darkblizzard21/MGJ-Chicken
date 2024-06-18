#pragma once
#include <chrono>
#include <string>

class Timer {
public:
    Timer(const std::string& name = "");
    ~Timer();

    void finish();
    float elapsedSeconds();

    bool hasEnded() const;
    bool print = true;
private:
    bool ended = false;
    const std::chrono::steady_clock::time_point start_; 
    std::chrono::steady_clock::time_point end_;
    const std::string                           name_;
};