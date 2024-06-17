#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <Shader.h>
#include <vector>
#include <Texture.h>

class QuadManager;

class Quad final
{
private:
	friend QuadManager;
	Quad() = default;

public:
	glm::vec2 position = { 0, 0 };
	glm::vec2 scale = { 1,1 };
	float rotation = 0;
	std::shared_ptr<Texture> colorTexture;
};

class QuadManager
{

public:
	QuadManager() = default;
	~QuadManager();
	void Initialize();

	std::shared_ptr<Quad> CreateQuad();
	std::shared_ptr<Quad> CreateQuad(std::shared_ptr<Texture> colorTexture);

	void RenderQuads();

private:
	void InitializeMesh();
	void InitializeShader();
	void InitializeTexture();

	bool initialized = false;
	std::unique_ptr<Shader> shader;
	std::shared_ptr<Texture> defaultColorTexture;

	unsigned int VBO;
	unsigned int VAO; 
	unsigned int EBO;

	std::vector<std::shared_ptr<Quad>> quads;

};