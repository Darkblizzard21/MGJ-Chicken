#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <Shader.h>
#include <vector>
#include <Texture.h>
#include <UberShader.h>

class QuadManager;

struct Quad final : public UberData
{
private:
	friend QuadManager;
	Quad() = default;
};

class QuadManager
{

public:
	QuadManager() = default;
	~QuadManager();
	void Initialize();

	std::shared_ptr<Quad> CreateQuad();
	std::shared_ptr<Quad> CreateQuad(std::shared_ptr<Texture> colorTexture, std::shared_ptr<Texture> normalTexture = nullptr);


	void RenderQuads();

private:
	bool initialized_ = false;
	MeshBuffers buffers_;

	std::vector<std::shared_ptr<Quad>> quads_;

};