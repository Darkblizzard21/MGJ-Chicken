#include "Quad.h"


#include <glad/glad.h>

QuadManager::~QuadManager()
{
	buffers.cleanUp();
}

void QuadManager::Initialize()
{
	InitializeMesh();
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

std::shared_ptr<Quad> QuadManager::CreateQuad(std::shared_ptr<Texture> colorTexture, std::shared_ptr<Texture> normalTexture)
{
	auto res = CreateQuad();
	res->colorTexture = colorTexture;
	res->normalTexture = normalTexture;
	return res;
}

void QuadManager::RenderQuads()
{
	if (!initialized) throw "no initialized";
	std::vector<size_t> toremove = {};
	for (size_t i = 0; i < quads.size(); i++)
	{
		if (quads[i].use_count() == 1) {
			toremove.push_back(i);
			continue;
		}

		UberShader::DrawElements(*quads[i].get(), buffers);
	}
	
	size_t offset = 0;
	for (size_t i = 0; i < toremove.size(); i++)
	{
		quads.erase(quads.begin() + (toremove[i] - offset));
		offset++;
	}
}

void QuadManager::InitializeMesh()
{
	std::vector<UberVertex> vertices = {
	// position | textcoord |
	UberVertex(0.5f,  0.5f, 1.0f, 0.0f), // top right
	UberVertex(0.5f, -0.5f, 1.0f, 1.0f), // bottom right
	UberVertex(-0.5f, -0.5f, 0.0f, 1.0f), // bottom left
	UberVertex(-0.5f,  0.5f, 0.0f, 0.0f), // top left 
	};
	std::vector<unsigned int> indices = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	buffers = UberShader::UploadMesh(vertices, indices);
}

void QuadManager::InitializeTexture()
{
	defaultColorTexture = std::make_shared<Texture>("sheep.png");
}

