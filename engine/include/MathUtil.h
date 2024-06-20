#pragma once
#include <glm/glm.hpp>

inline glm::vec2 rotate(const glm::vec2& v, const float r) {
	const float sin = glm::sin(r);
	const float cos = glm::cos(r);
	return glm::vec2(
		cos * v.x - sin * v.y,
		sin * v.x + cos * v.y
	);
}