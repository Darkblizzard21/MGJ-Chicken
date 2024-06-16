#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <Shader.h>
#include <vector>

class QuadManager;

class Quad
{
private:
	friend QuadManager;
	Quad() = default;

public:
	glm::vec2 position = { 0, 0 };
	glm::vec2 scale = { 1,1 };
	float rotation = 0;

	std::shared_ptr<Shader> shader;
};

class QuadManager
{

public:
	QuadManager();
	~QuadManager();

	std::shared_ptr<Quad> CreateQuad();

	void RenderQuads();


private:

	unsigned int VBO;
	unsigned int VAO;

	std::vector<std::shared_ptr<Quad>> quads;

};