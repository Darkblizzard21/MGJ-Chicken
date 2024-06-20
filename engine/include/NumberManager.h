#pragma once
#include "UberShader.h"
#include "Quad.h"

class NumberManager;

struct Number {
	uint32_t GetNumber() const;
	void SetNumber(const uint32_t& num);
	void SetPos(const glm::vec2& pos);
	void SetScale(const glm::vec2& scale);
	void SetPadding(const float& padding);
private:
	friend NumberManager;
	Number() = default;

	uint32_t num_ = 0;

	float padding_ = 0.5f;
	glm::vec2 pos_ = glm::vec2(0, 0);
	glm::vec2 scale_ = glm::vec2(1, 1);

	bool dirty_ = true;

	std::vector< std::shared_ptr<Quad>> quads_ = {};
};

class NumberManager
{
public:
	NumberManager() = default;
	~NumberManager() = default;
	void Initialize(QuadManager* uiQuadManager);

	std::shared_ptr<Number> CreateNumber();

	void Update();
private:
	QuadManager* quadManager = nullptr;
	std::shared_ptr<Texture> numberTextureSheet;

	std::vector<std::shared_ptr<Number>> numbers_;
};
