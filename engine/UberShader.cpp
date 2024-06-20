#include "UberShader.h"
#include <glad/glad.h>


std::unique_ptr<Shader> UberShader::uber = nullptr;
std::unique_ptr<Texture> UberShader::fallbackTexture = nullptr;
std::unique_ptr<Texture> UberShader::fallbackNormals = nullptr;
glm::vec2 UberShader::cameraPosition = { 0,4.5 };

UberVertex::UberVertex() {
	this->pos = glm::vec2(0, 0);
	this->tex = glm::vec2(0, 0);
	this->normal = glm::vec3(0, 0, 1.f);
}

UberVertex::UberVertex(const glm::vec2& pos)
{
	this->pos = pos;
	this->tex = pos;
	tex.y = -tex.y;
	normal = glm::vec3(0, 0, 1.f);
}

UberVertex::UberVertex(const float& x, const float& y)
{
	pos.x = x;
	pos.y = y;
	tex.x = x;
	tex.y = -y;
	normal = glm::vec3(0, 0, 1.f);
}

UberVertex::UberVertex(const glm::vec2& pos, const glm::vec2& tex)
{
	this->pos = pos;
	this->tex = tex;
	normal = glm::vec3(0, 0, 1.f);
}

UberVertex::UberVertex(const float& x, const float& y, const float& u, const float& v)
{
	pos.x = x;
	pos.y = y;
	tex.x = u;
	tex.y = v;
	normal = glm::vec3(0, 0, 1.f);
}

UberVertex::UberVertex(const glm::vec2& pos, const glm::vec2& tex, const glm::vec3& normal)
{
	this->pos = pos;
	this->tex = tex;
	this->normal = normal;
}

void UberShader::Initialize()
{
	if (uber != nullptr) {
		return;
	}


	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTex;\n"
		"layout (location = 2) in vec3 aNormal;\n"
		"uniform vec2  cameraOffset;\n"
		"uniform vec2  pos; \n"
		"uniform vec2  scale; \n"
		"uniform vec2  uvOffset;\n"
		"uniform vec2  uvScale;\n"
		"uniform int   uvGridSize;\n"
		"uniform int   uvTile;\n"
		"\n"
		"uniform float sinR; \n"
		"uniform float cosR; \n"
		"uniform float depth;\n"
		"\n"
		"uniform bool useNormalTex;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"out vec3 Normal;\n"
		"\n"
		"void main()\n"
		"{\n"
		// Setup rotation
		// world pos
		"   float rotX = cosR * aPos.x - sinR * aPos.y;\n"
		"   float rotY = sinR * aPos.x + cosR * aPos.y;\n"
		"	vec2 wPos = vec2(rotX, rotY) * scale + pos;\n"
		"   vec2 sPos =  (wPos + cameraOffset) * vec2(1.0/8.0, 1.0/4.5);\n"
		"   gl_Position = vec4(sPos.x, sPos.y, depth, 1.0);\n"
		// texture coords
		"   if(uvGridSize == 1)\n"
		"	{\n"
		"		TexCoord = (aTex + uvOffset) * uvScale;\n"
		"	} else {\n"
		"		vec2 uv = (aTex + uvOffset) * uvScale;\n"
		"		float tileSize = 1.f / uvGridSize;\n"
		"		int tileX = uvTile % uvGridSize;\n"
		"		int tileY = uvTile / uvGridSize;\n"
		"		TexCoord = vec2(tileX * tileSize + uv.x / uvGridSize, tileY * tileSize + uv.y / uvGridSize);\n"
		"	}\n"
	    // Normals
		"   if(!useNormalTex)\n"
		"	{\n"
		"		float nRotX = cosR * aNormal.x - sinR * aNormal.y;\n"
		"		float nRotY = sinR * aNormal.x + cosR * aNormal.y;\n"
		"		Normal      = vec3(nRotX, nRotY, aNormal.z);\n"
		"	} else {\n"
		"		Normal      = vec3(0,0,1);"
		"	}\n"
		"}\0";

	const std::string fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"in vec2 FragPos;\n"
		"in vec3 Normal;\n"
		"\n"
		"uniform bool useAlpha;\n"
		"uniform float alphaThreshold;\n"
		"\n"
		"uniform sampler2D ColorTex;\n"
		"\n"
		"uniform bool useNormalTex;\n"
		"uniform sampler2D NormalTex;\n"
		"uniform float sinR; \n"
		"uniform float cosR; \n"
		"uniform int depthInt;\n"
		"\n"
		"uniform vec3  tin; \n"
		"uniform float tinfluence;\n"
		"\n"
		"layout(location = 0) out vec4 gAlbedo;\n"
		"layout(location = 1) out vec3 gNormal;\n"
		"layout(location = 2) out float gLayer;\n"
		"\n"
		"void main()\n"
		"{\n"
		// alpha clipping
		"   float alpha = 1.f;"
		"   if(useAlpha)\n"
		"   {\n"
		"		alpha = texture(ColorTex, TexCoord).a;\n"
		"		if(alpha < alphaThreshold) discard;\n"
		"   }\n"
		// layer buffer
		"   gLayer = depthInt / 255.f;\n"
		// albedo
		"   if(tinfluence <= 0.f)\n"
		"   {\n"
		"		gAlbedo = vec4(texture(ColorTex, TexCoord).rgb, alpha);\n"
		"   } else if(tinfluence >= 1.f) {\n"
		"		gAlbedo =  vec4(tin, alpha);\n"
		"	} else {\n"
		"		gAlbedo = vec4(mix(texture(ColorTex, TexCoord).rgb, tin, tinfluence), alpha);\n"
		"   } \n"
		// normal
		"   if(useNormalTex)\n"
		"   {\n"
		"		vec3  normal = (texture(NormalTex, TexCoord).rgb - 0.5f) * 2;\n"
		"		float nRotX  = cosR * normal.x - sinR * normal.y;\n"
		"		float nRotY  = sinR * normal.x + cosR * normal.y;\n"
		"		gNormal      = vec3(nRotX, nRotY, normal.z);\n"
		"   } else {\n"
		"		gNormal = Normal;\n"
		"   }\n"
		"}\n\0";

	uber = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);

	std::vector<glm::vec3> fallback = { glm::vec3(1.0, 0.301f, 0) , glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0, 0.301f, 0) };
	fallbackTexture = std::make_unique<Texture>(fallback, 2, 2, SamplerTypes::NearestNeighbour);
	fallback = { glm::vec3(0.5f, 0.5f, 1.f) };
	fallbackNormals = std::make_unique<Texture>(fallback, 1, 1, SamplerTypes::NearestNeighbour);
}

void UberShader::DrawElements(const UberData& settings, const MeshBuffers& buffers)
{
	DrawElements(settings, buffers.VAO, buffers.triangleCount);
}

void UberShader::DrawElements(const UberData& settings, const unsigned int& VAO, const unsigned int& triangles)
{
	assert(uber != nullptr);
	if (!settings.draw) return;

	Texture* colorTex = settings.colorTexture != nullptr ? settings.colorTexture.get() : fallbackTexture.get();
	Texture* normalTex = settings.normalTexture != nullptr ? settings.normalTexture.get() : fallbackNormals.get();
	uber->use();
	uber->setVec2("cameraOffset", -cameraPosition);
	uber->setVec2("pos", settings.position);
	uber->setVec2("scale", settings.scale);
	uber->setFloat("sinR", glm::sin(settings.rotation));
	uber->setFloat("cosR", glm::cos(settings.rotation));
	float depth = 1 - ((settings.layer * 0.9f / std::numeric_limits<uint8_t>::max()) + 0.05f);
	uber->setFloat("depth", depth);
	uber->setInt("depthInt", 255-settings.layer);

	uber->setBool("useAlpha", settings.useAlpha && colorTex->hasAlpha());
	uber->setFloat("alphaThreshold", settings.alphaThreshold);


	uber->setVec3("tin", settings.tin);
	uber->setFloat("tinfluence", settings.tinfluence);

	uber->setVec2("uvOffset", settings.uvOffset);
	uber->setVec2("uvScale", settings.uvScale);
	const auto gSize = glm::max(1, settings.uvGridSize);
	uber->setInt("uvGridSize", gSize);
	uber->setInt("uvTile", settings.uvTile % (gSize * gSize));

	uber->setInt("ColorTex", 0);
	colorTex->Bind(0);
	uber->setBool("useNormalTex", settings.useNormalTex);
	uber->setInt("NormalTex", 1);
	normalTex->Bind(1);

	if (settings.wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, 0);
	// maybe remove
	glBindVertexArray(0);
}

bool UberShader::InFrustum(const glm::vec2& point)
{
	const auto p = glm::abs(point - cameraPosition);
	return p.x <= 8.0f && p.y <= 4.5f;
}

bool UberShader::InFrustum(const MeshBuffers& mesh, const glm::vec2& offset)
{
	return InFrustum(mesh.lower + offset, mesh.upper + offset);
}

bool UberShader::InFrustum(const glm::vec2& lower, const glm::vec2& upper)
{
	const auto lowerCorner = cameraPosition - glm::vec2(8.f, 4.5f);
	const auto upperCorner = cameraPosition + glm::vec2(8.f, 4.5f);
	return lower.x <= upperCorner.x && lower.y <= upperCorner.y && lowerCorner.x < upper.x && lowerCorner.y < upper.y;
}

MeshBuffers UberShader::UploadMesh(const std::vector<UberVertex>& vertexBuffer, const std::vector<unsigned int>& indexBuffer) {
	MeshBuffers result = {};
	assert(indexBuffer.size() % 3 == 0);
	result.triangleCount = indexBuffer.size() / 3;

	glGenBuffers(1, &result.VBO);
	glGenVertexArrays(1, &result.VAO);
	glGenBuffers(1, &result.EBO);
	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(result.VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UberVertex) * vertexBuffer.size(), vertexBuffer.data(), GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexBuffer.size(), indexBuffer.data(), GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers

	// Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// TexCords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);

	result.lower = glm::vec2(std::numeric_limits<float>::max());
	result.upper = glm::vec2(std::numeric_limits<float>::lowest());
	for (const auto& vert : vertexBuffer)
	{
		result.lower = glm::min(result.lower, vert.pos);
		result.upper = glm::max(result.upper, vert.pos);
	}

	return result;
}


bool MeshBuffers::valid()
{
	return VBO != -1 && VAO != -1 && EBO != -1 && triangleCount > 0;
}

void MeshBuffers::cleanUp()
{
	// cleanup
	if (VBO != -1) {
		glDeleteBuffers(1, &VBO);
	}
	if (VAO != -1) {
		glDeleteBuffers(1, &VAO);
	}
	if (EBO != -1) {
		glDeleteBuffers(1, &EBO);
	}

	VBO = -1;
	VAO = -1;
	EBO = -1;
	triangleCount = 0;
}
