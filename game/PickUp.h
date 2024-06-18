#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include <box2d/box2d.h>

class PickUp {
public:

	PickUp();
	void update();
	std::shared_ptr<Quad> quad;

private:
	float yVelocity = 0;
	bool wasSpacePressed = false;

};