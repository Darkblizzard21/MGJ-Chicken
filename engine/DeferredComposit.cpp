#include "DeferredComposit.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include "UberShader.h"
#include <iostream>

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

	const std::string maxNum = std::to_string(maxLightCount);
	const std::string fragmentUniforms = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"\n"
		"uniform sampler2D ColorTex;\n"
		"uniform sampler2D NormalTex;\n"
		"uniform sampler2D LayerTex;\n"
		"\n"
		"uniform int   lightCount;"
		"uniform vec3  lightColor[" + maxNum + "];\n"
		"uniform vec2  lightPos[" + maxNum + "];\n"
		"uniform float lightRadius[" + maxNum + "];\n"
		"\n"
		"out vec4 FragColor;\n";

	const std::string fragmentMain = ""
		"   vec3 N = texture(NormalTex, TexCoord).rgb;\n"
		// reconstruct screen space
		"   vec2 screenSpace = (TexCoord - vec2(0.5f)) * 2;\n"
		"   vec2 cameraSpace = screenSpace * vec2(8.0, 4.5);\n"
		// lighting
		"vec3 color = vec3(0.1);\n"
		"for (int i = 0; i < lightCount; i++) {\n"
		"   vec2  p = lightPos[i];\n"
		"   float r = lightRadius[i];\n"
		"   float distance = length(cameraSpace-p);\n"
		"   if(distance <= lightRadius[i])\n"
		"   {\n"
		"		vec3 c = lightColor[i];\n"
		// Falloff
		"		float falloff = distance / r;\n"
		"		falloff = 1-falloff;\n"
		"		falloff = (falloff * falloff + falloff) * 0.5f;\n"
		// phong lighting
		"       vec3 L = vec3(normalize(p-cameraSpace), 0);\n"
		"		float lambertian = max(dot(N, L), 0.0);\n"
		"       float albedo = min(lambertian + 0.5f, 1.f);\n"
		"		color = color + c * (2* albedo * falloff);\n"
		"   }\n"
		"}\n"
		"\n"
		"FragColor = vec4(texture(ColorTex, TexCoord).rgb * color + 0.5f * color * color,1);\n";
		"FragColor = vec4(color,1);\n";

	const std::string fragmentShaderSource = fragmentUniforms +
		"void main()\n"
		"{\n"
		+ fragmentMain +
		"}\n\0";

	pass = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);

	const std::string debugfragmentShaderSource = fragmentUniforms +
		"void main()\n"
		"{\n"
		"	vec2 tCord2 = vec2(mod(TexCoord.x, 0.5f), mod(TexCoord.y, 0.5f)) * 2;\n"
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

	// reserve vectors
	pointLights.reserve(maxLightCount);

	lightPosV.reserve(maxLightCount);
	lightColorV.reserve(maxLightCount);
	lightRadiusV.reserve(maxLightCount);
	toDelete.reserve(maxLightCount);
}

void DeferredCompositPass::Execute(const int& colorTex, const int& normalTex, const int& posTex)
{
	assert(pass != nullptr);
	Shader* ptr = debug ? debugPass.get() : pass.get();
	ptr->use();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	ptr->setInt("ColorTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	ptr->setInt("NormalTex", 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, posTex);
	ptr->setInt("LayerTex", 2);


	UpdateWorkingVars();
	ptr->setInt("lightCount", pointLights.size());
	ptr->setVec3V("lightColor", lightColorV);
	ptr->setVec2V("lightPos", lightPosV);
	ptr->setFloatV("lightRadius", lightRadiusV);

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

std::shared_ptr<PointLight> DeferredCompositPass::CreatePointLight()
{
	if (maxLightCount <= pointLights.size()) {
		std::cout << "The max amount of pointlights is already reached. Handing out pointer to first pointight again." << std::endl;
		return pointLights[0];
	}

	PointLight* ptr = new PointLight();
	std::shared_ptr<PointLight> light = std::shared_ptr<PointLight>(ptr);
	pointLights.push_back(light);
	return light;
}

void DeferredCompositPass::UpdateWorkingVars()
{

	lightPosV.clear();
	lightColorV.clear();
	lightRadiusV.clear();

	std::vector<size_t> toremove = {};
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		if (pointLights[i].use_count() == 1) {
			toremove.push_back(i);
			continue;
		}

		// do shit
		lightPosV.push_back(pointLights[i]->lightPos - UberShader::cameraPosition);
		lightColorV.push_back(pointLights[i]->lightColor);
		lightRadiusV.push_back(pointLights[i]->lightRadius);
	}

	size_t offset = 0;
	for (size_t i = 0; i < toremove.size(); i++)
	{
		pointLights.erase(pointLights.begin() + (toremove[i] - offset));
		offset++;
	}
	toDelete.clear();
}
