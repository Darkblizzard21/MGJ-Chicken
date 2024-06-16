#pragma once
#include <glm/glm.hpp>
#include "Quad.h"

class Minecart {
public:

	Minecart();
	void update();

private:
	float yVelocity = 0;
	std::shared_ptr<Quad> quad;
	bool wasSpacePressed;
};