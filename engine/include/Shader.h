#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader {
public:
    // constructor reads and builds the shader
    Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

    // deletes the shaders
    ~Shader();

    // use/activate the shader
    void use();
    // utility uniform functions we can expand them later as we need them
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4x4(const std::string& name, const glm::mat4x4& value) const;

    void setFloatV(const std::string& name, const std::vector<float>& value) const;
    void setVec2V(const std::string& name, const std::vector<glm::vec2>& value) const;
    void setVec3V(const std::string& name, const std::vector<glm::vec3>& value) const;

    // getFunctions
    unsigned int getProgramId() const;
    unsigned int getVertexShaderId() const;
    unsigned int getFragmentShaderId() const;

private:
    // the program ID
    unsigned int ID         = -1;
    unsigned int vertexID   = -1;
    unsigned int fragmentID = -1;
};
