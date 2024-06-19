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

#ifdef DEBUG
	inline void FlipDebug() { debug = !debug; }
	inline void EnableDebug() { debug = true; }
	inline void DisableDebug() { debug = false; }
#else
	inline void FlipDebeg() {}
	inline void EnableDebug() {}
	inline void DisableDebug() {}
#endif

	std::shared_ptr<PointLight> CreatePointLight();

private:
	void UpdateWorkingVars();

	void setInt(const std::string& name, const int& value);
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;

	void setFloatV(const std::string& name, const std::vector<float>& value) const;
	void setVec2V(const std::string& name, const std::vector<glm::vec2>& value) const;
	void setVec3V(const std::string& name, const std::vector<glm::vec3>& value) const;

	std::unique_ptr<Shader> pass = nullptr;

#ifdef DEBUG
	bool debug = false;
	std::unique_ptr<Shader> debugPass = nullptr;
#endif

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
