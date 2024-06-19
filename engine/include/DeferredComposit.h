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

#ifdef DEBUG
	inline void FlipDebug() { debug = !debug; }
	inline void EnableDebug() { debug = true; }
	inline void DisableDebug() { debug = false; }
#else
	inline void FlipDebeg() {}
	inline void EnableDebug() {}
	inline void DisableDebug() {}
#endif
private:
	std::unique_ptr<Shader> pass = nullptr;

	glm::vec2 lightPos = glm::vec2(0, 5);
	glm::vec3 lightColor = glm::vec3(1, 1, 1);
	float ligthSize = 5;
#ifdef DEBUG
	bool debug = false;
	std::unique_ptr<Shader> debugPass = nullptr;
#endif

	unsigned int VBO = -1;
	unsigned int VAO = -1;
	unsigned int EBO = -1;
};
