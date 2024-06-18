#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>

enum class SamplerTypes
{
	Linear,
	NearestNeighbour
};

class Texture {
public:
	Texture(const std::string& texture, SamplerTypes magFilter = SamplerTypes::Linear);
	Texture(const std::vector<glm::vec3>& data, const size_t& width, const size_t& height, SamplerTypes magFilter = SamplerTypes::Linear);
	Texture(const std::vector<glm::vec4>& data, const size_t& width, const size_t& height, SamplerTypes magFilter = SamplerTypes::Linear);

	~Texture();

	void Bind(const unsigned int location = 0);
	bool hasAlpha();
private:
	void SetupSampler(SamplerTypes magFilter);

	unsigned int id;
	bool hasAlpha_;
};