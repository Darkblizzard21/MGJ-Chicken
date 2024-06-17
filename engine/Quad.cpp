#include "Quad.h"


#include <glad/glad.h>

QuadManager::~QuadManager()
{
	if (initialized) {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(2, &VAO);
	}
}

void QuadManager::Initialize()
{
	InitializeMesh();
	InitializeShader();
	InitializeTexture();
	initialized = true;
}

std::shared_ptr<Quad> QuadManager::CreateQuad()
{
	if (!initialized) throw "no initialized";
	Quad* ptr = new Quad();
	ptr->colorTexture = defaultColorTexture;
	std::shared_ptr<Quad> quad = std::shared_ptr<Quad>(ptr);
	quads.push_back(quad);
	return quad;
}

std::shared_ptr<Quad> QuadManager::CreateQuad(std::shared_ptr<Texture> colorTexture)
{
	auto res = CreateQuad();
	res->colorTexture = colorTexture;
	return res;
}

void QuadManager::RenderQuads()
{
	if (!initialized) throw "no initialized";
	std::vector<size_t> toremove;
	for (size_t i = 0; i < quads.size(); i++)
	{
		if (quads[i].use_count() == 1) {
			toremove.push_back(i);
			continue;
		}
		shader->use();
		shader->setVec2("pos", quads[i]->position);
		shader->setVec2("scale", quads[i]->scale);
		shader->setFloat("rotation", quads[i]->rotation);

		shader->setInt("ColorTex", 0);
		quads[i]->colorTexture->Bind(0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void QuadManager::InitializeMesh()
{
	float vertices[] = {
	// position | textcoord |
	 0.5f,  0.5f, 1.0f, 0.0f, // top right
	 0.5f, -0.5f, 1.0f, 1.0f, // bottom right
	-0.5f, -0.5f, 0.0f, 1.0f, // bottom left
	-0.5f,  0.5f, 0.0f, 0.0f, // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers

	// Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// TexCords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void QuadManager::InitializeShader()
{
	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTex;\n"
		"uniform vec2  pos; \n"
		"uniform vec2  scale; \n"
		"uniform float rotation; \n"
		"\n"
		"out vec2 TexCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   float rotX = cos(rotation) * aPos.x - sin(rotation) * aPos.y;\n"
		"   float rotY = sin(rotation) * aPos.x + cos(rotation) * aPos.y;\n"
		"	vec2 wPos = vec2(rotX, rotY) * scale + pos;\n"
		"   gl_Position = vec4(wPos.x, wPos.y, 0.0f, 1.0);\n"
		"   TexCoord = aTex;\n"
		"}\0";
	const std::string fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"\n"
		"uniform sampler2D ColorTex;\n"
		"\n"
		"out vec4 FragColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(texture(ColorTex, TexCoord).rgb,1);\n"
		"}\n\0";

	shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
}

void QuadManager::InitializeTexture()
{
	defaultColorTexture = std::make_shared<Texture>("sheep.png");
}

