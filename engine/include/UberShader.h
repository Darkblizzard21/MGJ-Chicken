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
	glm::vec2 lower;
	glm::vec2 upper;

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
	std::shared_ptr<Texture> normalTexture = nullptr;
	bool useNormalTex = true;

	glm::vec2 uvOffset = { 0,0 };
	glm::vec2 uvScale = { 1,1 };

	bool useAlpha = true;
	float alphaThreshold = 0.5f;

	uint8_t layer = 128;

	bool wireframe = false;
};

struct UberVertex {
	glm::vec2 pos;
	glm::vec2 tex;
	glm::vec3 normal;

	UberVertex();
	// Initalizes tex and pos on the same value and default normal
	UberVertex(const glm::vec2& pos);
	UberVertex(const float& x, const float& y);
	// Initalizes pos and tex and default normal
	UberVertex(const glm::vec2& pos, const glm::vec2& tex);
	UberVertex(const float& x, const float& y, const float& u, const float& v);
	// Initalizes pos and tex and normal
	UberVertex(const glm::vec2& pos, const glm::vec2& tex, const glm::vec3& normal);
};

class UberShader
{
private:
	UberShader() = delete;
	static std::unique_ptr<Shader> uber;
	static std::unique_ptr<Texture> fallbackTexture;
	static std::unique_ptr<Texture> fallbackNormals;

public:
	static glm::vec2 cameraPosition;

	static void Initialize();
	static void DrawElements(const UberData& settings, const MeshBuffers& buffers);
	static void DrawElements(const UberData& settings, const unsigned int& VAO, const unsigned int& tirangles);

	static bool InFrustum(const glm::vec2& point);
	static bool InFrustum(const MeshBuffers& mesh, const glm::vec2& offset);
	static bool InFrustum(const glm::vec2& lower, const glm::vec2& upper);

	static MeshBuffers UploadMesh(const std::vector<UberVertex>& vertexBuffer, const std::vector<unsigned int>& indexBuffer);
};