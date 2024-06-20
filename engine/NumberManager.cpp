#include "NumberManager.h"


uint32_t Number::GetNumber() const
{
	return num_;
}

void Number::SetNumber(const uint32_t& num)
{
	if (num_ != num) {
		dirty_ = true;
	}
	num_ = num;
}

void Number::SetPos(const glm::vec2& pos) {
	pos_ = pos;
	dirty_ = true;
}

void Number::SetScale(const glm::vec2& scale) {
	scale_ = scale;
	dirty_ = true;
}

void Number::SetPadding(const float& padding) {
	padding_ = padding;
	dirty_ = true;
}

void NumberManager::Initialize(QuadManager* uiQuadManager)
{
	quadManager = uiQuadManager;
	for (size_t i = 0; i < 10; i++)
	{
		digitTextures_[i] = std::make_shared<Texture>(std::to_string(i) + ".png");
	}
}

std::shared_ptr<Number> NumberManager::CreateNumber()
{
	if (!quadManager) throw "no initialized";
	Number* ptr = new Number();
	std::shared_ptr<Number> quad = std::shared_ptr<Number>(ptr);
	numbers_.push_back(quad);
	return quad;
}

void NumberManager::Update()
{
	if (!quadManager) throw "no initialized";

	std::vector<size_t> toremove = {};
	for (size_t i = 0; i < numbers_.size(); i++)
	{
		auto& number = numbers_[i];
		if (number.use_count() == 1) {
			toremove.push_back(i);
			continue;
		}
		if (!number->dirty_) {
			continue;
		}

		// generate Digits
		uint32_t num = number->num_;
		std::vector<uint8_t> digits = {};
		do
		{
			digits.push_back(num % 10);
			num = num / 10;
		} while (num != 0);

		// resize quads to digits
		while (number->quads_.size() < digits.size())
		{
			number->quads_.push_back(quadManager->CreateQuad());
		}
		while (number->quads_.size() > digits.size())
		{
			number->quads_.pop_back();
		}

		// set digit textures and pos and co
		for (size_t d = 0; d < digits.size(); d++)
		{
			const float xOffset = number->padding_ * number->scale_.x * d;
			number->quads_[d]->position = number->pos_ - glm::vec2(xOffset, 0);
			number->quads_[d]->scale = number->scale_;
			number->quads_[d]->colorTexture = digitTextures_[digits[d]];

		}

		number->dirty_ = false;
	}

	size_t offset = 0;
	for (size_t i = 0; i < toremove.size(); i++)
	{
		numbers_.erase(numbers_.begin() + (toremove[i] - offset));
		offset++;
	}
}
