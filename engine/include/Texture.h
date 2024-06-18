#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>

class Texture {
public:
    Texture(const std::string& texture);
    Texture(const std::vector<glm::vec3>& data, const size_t& width, const size_t& height);
    Texture(const std::vector<glm::vec4>& data, const size_t& width, const size_t& height);

    ~Texture();

    void Bind(const unsigned int location = 0);
    bool hasAlpha();
private:
    void SetupSampler();

    unsigned int id;
    bool hasAlpha_;
};