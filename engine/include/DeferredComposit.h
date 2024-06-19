#pragma once
#include <memory>
#include "Shader.h"

class DeferredCompositPass
{
public:
	DeferredCompositPass() = default;
	~DeferredCompositPass();

	void Initalize();
	void Execute(const int& colorTex, const int& normalTex, const int& posTex);

	void CleanUp();
private:
	std::unique_ptr<Shader> pass = nullptr;

	unsigned int VBO = -1;
	unsigned int VAO = -1;
	unsigned int EBO = -1;
};
