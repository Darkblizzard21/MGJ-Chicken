#include "UberShader.h"
#include <glad/glad.h>


std::unique_ptr<Shader> UberShader::uber = nullptr;
std::unique_ptr<Texture> UberShader::fallbackTexture = nullptr;
glm::vec2 UberShader::cameraPosition = { 0,0 };

UberVertex::UberVertex(const glm::vec2& pos)
{
	this->pos = pos;
	this->tex = pos;
	tex.y = -tex.y;
}

UberVertex::UberVertex(const float& x, const float& y)
{
	pos.x = x;
	pos.y = y;
	tex.x = x;
	tex.y = -y;
}

UberVertex::UberVertex(const glm::vec2& pos, const glm::vec2& tex)
{
	this->pos = pos;
	this->tex = tex;
}

UberVertex::UberVertex(const float& x, const float& y, const float& u, const float& v)
{
	pos.x = x;
	pos.y = y;
	tex.x = u;
	tex.y = v;
}

void UberShader::Initialize()
{
	if (uber != nullptr) {
		return;
	}


	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTex;\n"
		"uniform vec2  pos; \n"
		"uniform vec2  scale; \n"
		"uniform vec2  uvOffset;\n"
		"uniform vec2  uvScale;\n"
		"uniform float rotation; \n"
		"uniform float depth;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   float rotX = cos(rotation) * aPos.x - sin(rotation) * aPos.y;\n"
		"   float rotY = sin(rotation) * aPos.x + cos(rotation) * aPos.y;\n"
		"	vec2 wPos = vec2(rotX, rotY) * scale + pos;\n"
		"   vec2 sPos =  (wPos + vec2(0,-4.5)) * vec2(1.0/8.0, 1.0/4.5);\n"
		"   gl_Position = vec4(sPos.x, sPos.y, depth, 1.0);\n"
		"   TexCoord = (aTex + uvOffset) * uvScale;\n"
		"}\0";
	const std::string fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"\n"
		"uniform bool useAlpha;\n"
		"uniform float alphaThreshold;\n"
		"uniform sampler2D ColorTex;\n"
		"\n"
		"out vec4 FragColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   if(useAlpha)\n"
		"   {\n"
		"		float alpha = texture(ColorTex, TexCoord).a;\n"
		"		if(alpha < alphaThreshold) discard;\n"
		"   }\n"
		"   FragColor = vec4(texture(ColorTex, TexCoord).rgb,1);\n"
		"}\n\0";

	uber = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);

	std::vector<glm::vec3> fallback = { glm::vec3(1.0, 0.301f, 0) };
	fallbackTexture = std::make_unique<Texture>(fallback, 1, 1);
}

void UberShader::DrawElements(const UberData& settings, const MeshBuffers& buffers)
{
	DrawElements(settings, buffers.VAO, buffers.triangleCount);
}

void UberShader::DrawElements(const UberData& settings, const unsigned int& VAO, const unsigned int& triangles)
{
	assert(uber != nullptr);
	Texture* colorTex = settings.colorTexture != nullptr ? settings.colorTexture.get() : fallbackTexture.get();

	uber->use();
	uber->setVec2("pos", settings.position - cameraPosition);
	uber->setVec2("scale", settings.scale);
	uber->setFloat("rotation", settings.rotation);
	uber->setFloat("depth", 1.f - (settings.layer * 1.f / std::numeric_limits<uint8_t>::max()));

	uber->setBool("useAlpha", settings.useAlpha && colorTex->hasAlpha());
	uber->setFloat("alphaThreshold", settings.alphaThreshold);

	uber->setVec2("uvOffset", settings.uvOffset);
	uber->setVec2("uvScale", settings.uvScale);
	uber->setInt("ColorTex", 0);
	colorTex->Bind(0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, 0);
	// maybe remove
	glBindVertexArray(0);
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// TexCords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
