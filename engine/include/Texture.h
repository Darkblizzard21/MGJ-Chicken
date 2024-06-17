#pragma once
#include <string>

class Texture {
public:
    Texture(const std::string& texture);
    ~Texture();

    void Bind(const unsigned int location = 0);
private:
    unsigned int id;
};