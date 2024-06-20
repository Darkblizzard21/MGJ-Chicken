#include "Quad.h"


#include <glad/glad.h>

QuadManager::~QuadManager()
{
	buffers_.cleanUp();
}

void QuadManager::Initialize()
{
	if (initialized_) return;
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

	buffers_ = UberShader::UploadMesh(vertices, indices);

	initialized_ = true;
}

std::shared_ptr<Quad> QuadManager::CreateQuad()
{
	if (!initialized_) throw "no initialized";
	Quad* ptr = new Quad();
	std::shared_ptr<Quad> quad = std::shared_ptr<Quad>(ptr);
	quads_.push_back(quad);
	return quad;
}

std::shared_ptr<Quad> QuadManager::CreateQuad(std::shared_ptr<Texture> colorTexture, std::shared_ptr<Texture> normalTexture)
{
	if (!initialized_) throw "no initialized";
	auto res = CreateQuad();
	res->colorTexture = colorTexture;
	res->normalTexture = normalTexture;
	return res;
}

void QuadManager::RenderQuads()
{
	if (!initialized_) throw "no initialized";

	std::vector<size_t> toremove = {};
	for (size_t i = 0; i < quads_.size(); i++)
	{
		if (quads_[i].use_count() == 1) {
			toremove.push_back(i);
			continue;
		}

		UberShader::DrawElements(*quads_[i].get(), buffers_);
	}

	size_t offset = 0;
	for (size_t i = 0; i < toremove.size(); i++)
	{
		quads_.erase(quads_.begin() + (toremove[i] - offset));
		offset++;
	}
}
