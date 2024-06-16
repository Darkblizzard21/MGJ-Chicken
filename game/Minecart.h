#pragma once
#include <glm/glm.hpp>

class Minecart {
public:

	void update();

private:
	
	glm::vec2 position{ 0, 0 };
	float yVelocity = 0;
};