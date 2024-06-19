#include "DeferredComposit.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include "UberShader.h"

DeferredCompositPass::~DeferredCompositPass()
{
	CleanUp();
}

void DeferredCompositPass::Initalize()
{
	if (pass != nullptr) return;

	// set up shader
	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTex;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 0.5, 1.0);\n"
		"   TexCoord = aTex;\n"
		"}\0";


	const std::string fragmentUniforms = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"\n"
		"uniform sampler2D ColorTex;\n"
		"uniform sampler2D NormalTex;\n"
		"uniform sampler2D LayerTex;\n"
		"\n"
		"uniform vec3  lightColor;\n"
		"uniform vec2  lightPos;\n"
		"uniform float lightRadius;\n"
		"\n"
		"out vec4 FragColor;\n";

	const std::string fragmentMain = ""
		"   vec2 screenSpace = (TexCoord - vec2(0.5f)) * 2;\n"
		"   vec2 cameraSpace = screenSpace * vec2(8.0, 4.5);\n"
		"   float distance = length(cameraSpace-lightPos);\n"
		"   if(distance <= lightRadius)\n"
		"   {\n"
		"		float d = distance / lightRadius;\n"
		"		d = 1-d;\n"
		"		FragColor = vec4(texture(ColorTex, TexCoord).rgb * lightColor * d,1);\n"
		"   } else {\n"
		"		FragColor = vec4(0,0,0,1);\n"
		"   }\n";

	const std::string fragmentShaderSource = fragmentUniforms +
		"void main()\n"
		"{\n"
		+ fragmentMain +
		"}\n\0";

	pass = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);


#ifdef DEBUG
	const std::string debugfragmentShaderSource = fragmentUniforms +
		"void main()\n"
		"{\n"
		"	vec2 tCord2 = vec2(mod(TexCoord.x, 0.5f), mod(TexCoord.y, 0.5f)) * 2;"
		"	if(TexCoord.x < 0.5f)\n"
		"	{\n"
		"		if(TexCoord.y > 0.5f)\n"
		"		{\n"
		"			FragColor = vec4(texture(ColorTex, tCord2).rgb,1);\n"
		"		} else {\n"
		"			float l = texture(LayerTex, tCord2).r;\n"
		"			FragColor = vec4(l, l, l, 1);\n"
		"		}\n"
		"	} else {\n"
		"		if(TexCoord.y > 0.5f)\n"
		"		{\n"
		"			FragColor = vec4(texture(NormalTex, tCord2).rgb * 0.5f + 0.5f,1);\n"
		"		} else {\n"
		"			vec2 TexCoord = vec2(mod(TexCoord.x, 0.5f), mod(TexCoord.y, 0.5f)) * 2;"
		+ fragmentMain +
		"		}\n"
		"	}\n"
		"}\n\0";
	debugPass = std::make_unique<Shader>(vertexShaderSource, debugfragmentShaderSource);
#endif

	// set up mesh
	std::vector<glm::vec4> vertices = {
		// position | textcoord |
		glm::vec4(1.f,  1.f, 1.f, 1.f), // top right
		glm::vec4(1.f,  -1.f, 1.f, 0.f), // bottom right
		glm::vec4(-1.f,  -1.f, 0.f, 0.f), // bottom left
		glm::vec4(-1.f,  1.f, 0.f, 1.f), // top left 
	};
	std::vector<unsigned int> indices = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, vertices.data(), GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers

	// Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// TexCords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void DeferredCompositPass::Execute(const int& colorTex, const int& normalTex, const int& posTex)
{
	assert(pass != nullptr);

	pass->use();
#ifdef DEBUG
	if (debug) {
		debugPass->use();
	}
#endif

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	setInt("ColorTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	setInt("NormalTex", 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, posTex);
	setInt("LayerTex", 2);

	setVec3("lightColor", lightColor);
	setVec2("lightPos", lightPos - UberShader::cameraPosition);
	setFloat("lightRadius", lightRadius);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// maybe remove
	glBindVertexArray(0);
}

void DeferredCompositPass::CleanUp()
{
	pass = nullptr;
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
}

void DeferredCompositPass::setInt(const std::string& name, const int& value) {


#ifdef DEBUG
	if (debug) {
		debugPass->setInt(name, value);
	}
	else {
#endif 
		pass->setInt(name, value);
#ifdef DEBUG
	}
#endif
}

void DeferredCompositPass::setFloat(const std::string& name, float value) const
{
#ifdef DEBUG
	if (debug) {
		debugPass->setFloat(name, value);
	}
	else {
#endif 
		pass->setFloat(name, value);
#ifdef DEBUG
	}
#endif
}

void DeferredCompositPass::setVec2(const std::string& name, const glm::vec2& value) const
{
#ifdef DEBUG
	if (debug) {
		debugPass->setVec2(name, value);
	}
	else {
#endif 
		pass->setVec2(name, value);
#ifdef DEBUG
	}
#endif
}

void DeferredCompositPass::setVec3(const std::string& name, const glm::vec3& value) const
{
#ifdef DEBUG
	if (debug) {
		debugPass->setVec3(name, value);
	}
	else {
#endif 
		pass->setVec3(name, value);
#ifdef DEBUG
	}
#endif
}
