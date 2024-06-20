#pragma once
#include <memory>
#include <vector>
#include "Shader.h"

class DeferredCompositPass;

struct PointLight {
	glm::vec2 lightPos = glm::vec2(0, 5);
	glm::vec3 lightColor = glm::vec3(1, 1, 1);
	float lightRadius = 5;

private:
	friend DeferredCompositPass;
	PointLight() = default;
};

class DeferredCompositPass
{
public:
	static constexpr size_t maxLightCount = 32;

	DeferredCompositPass() = default;
	~DeferredCompositPass();

	void Initalize();
	void Execute(const int& colorTex, const int& normalTex, const int& posTex);

	void CleanUp();

	inline void FlipDebug() { debug = !debug; }
	inline void EnableDebug() { debug = true; }
	inline void DisableDebug() { debug = false; }
	inline bool GetDebug() { return debug; }

	std::shared_ptr<PointLight> CreatePointLight();

private:
	void UpdateWorkingVars();

	std::unique_ptr<Shader> pass = nullptr;

	bool debug = false;
	std::unique_ptr<Shader> debugPass = nullptr;

	unsigned int VBO = -1;
	unsigned int VAO = -1;
	unsigned int EBO = -1;

	std::vector<std::shared_ptr<PointLight>> pointLights;

	// working varaibles
	std::vector<std::shared_ptr<PointLight>> toDelete;
	std::vector<glm::vec2> lightPosV;
	std::vector<glm::vec3> lightColorV;
	std::vector<float> lightRadiusV;
};
