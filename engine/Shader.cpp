#include "Shader.h"

#include <glad/glad.h>

#include <iostream>

#include "glm/ext.hpp"

namespace {
	bool CompileShader(int shader)
	{
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			return false;
		}
		return true;
	}
}  // namespace

Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	// Vertex shader
	const char* vertexShaderSourcePtr = vertexShaderSource.c_str();

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexShaderSourcePtr, NULL);
	if (!CompileShader(vertexID)) {
		throw std::runtime_error("Vertex Shader did not compile!");
	}

	// Fragment shader
	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fragmentShaderSourcePtr, NULL);
	if (!CompileShader(fragmentID)) {
		throw std::runtime_error("Fragment Shader did not compile!");
	}

	// create Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertexID);
	glAttachShader(ID, fragmentID);
	glLinkProgram(ID);
	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAMM::LINK_FAILED\n" << infoLog << std::endl;
		throw std::runtime_error("Shader Programn did not link!");
	}
}

Shader::~Shader()
{
	glDeleteProgram(ID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
    if (location == -1) {
        std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
    }
#endif
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setMat4x4(const std::string& name, const glm::mat4x4& value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setFloatV(const std::string& name, const std::vector<float>& value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform1fv(location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setVec2V(const std::string& name, const std::vector<glm::vec2>& value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform2fv(location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setVec3V(const std::string& name, const std::vector<glm::vec3>& value) const
{
	const auto location = glGetUniformLocation(ID, name.c_str());
#ifdef DEBUG
	if (location == -1) {
		std::cout << "waring: There is no uniform named " << name << " on shader " << location << "." << std::endl;
	}
#endif
	glUniform3fv(location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
}

unsigned int Shader::getProgramId() const
{
	return ID;
}

unsigned int Shader::getVertexShaderId() const
{
	return vertexID;
}

unsigned int Shader::getFragmentShaderId() const
{
	return fragmentID;
}
