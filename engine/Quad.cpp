#include "Quad.h"


#include <glad/glad.h>

QuadManager::QuadManager()
{
	float vertices[] = {
		// first triangle
		 0.5f,  0.5f,  // top right
		 0.5f, -0.5f,  // bottom right
		-0.5f,  0.5f,  // top left 
		// second triangle
		 0.5f, -0.5f,  // bottom right
		-0.5f, -0.5f,  // bottom left
		-0.5f,  0.5f   // top left
	};

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

QuadManager::~QuadManager()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, &VAO);
}

std::shared_ptr<Quad> QuadManager::CreateQuad()
{
	Quad* ptr = new Quad();
	std::shared_ptr<Quad> quad = std::shared_ptr<Quad>(ptr);
	quads.push_back(quad);
	return quad;
}

void QuadManager::RenderQuads()
{
	std::vector<size_t> toremove;
	for (size_t i = 0; i < quads.size(); i++)
	{
		if (quads[i].use_count() == 1) {
			toremove.push_back(i);
			continue;
		}
		if (!quads[i]->shader) {
			printf("No shader set on quad");
			continue;
		}
		quads[i]->shader->use();
		quads[i]->shader->setVec2("pos", quads[i]->position);
		quads[i]->shader->setVec2("scale", quads[i]->scale);
		quads[i]->shader->setFloat("rotation", quads[i]->rotation);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

