#pragma once
#include "Shader.h"
#include <memory>
#include <Texture.h>
#include <vector>

struct MeshBuffers
{
	unsigned int VBO = -1;
	unsigned int VAO = -1;
	unsigned int EBO = -1;
	unsigned int triangleCount = 0;

	bool valid();
	void cleanUp();
};

struct UberData
{
	glm::vec2 position = { 0, 0 };
	glm::vec2 scale = { 1,1 };
	float rotation = 0;

	glm::vec3 tin = { 1,1,1 };
	float tinfluence = 0.f;

	std::shared_ptr<Texture> colorTexture = nullptr;
	glm::vec2 uvOffset = { 0,0 };
	glm::vec2 uvScale = { 1,1 };
};

struct UberVertex {
	glm::vec2 pos;
	glm::vec2 tex;

	// Initalizes tex and pos on the same value
	UberVertex(const glm::vec2& pos);
	UberVertex(const float& x, const float& y);
	// Initalizes pos and tex
	UberVertex(const glm::vec2& pos, const glm::vec2& tex);
	UberVertex(const float& x, const float& y, const float& u, const float& v);
};

class UberShader
{
private:
	UberShader() = delete;
	static std::unique_ptr<Shader> uber;
public:
	static void Initialize();
	static void DrawElements(const UberData& settings, const MeshBuffers& buffers);
	static void DrawElements(const UberData& settings, const unsigned int& VAO, const unsigned int& tirangles);

	static MeshBuffers UploadMesh(const std::vector<UberVertex>& vertexBuffer, const std::vector<unsigned int>& indexBuffer);
};