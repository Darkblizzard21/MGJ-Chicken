#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include <box2d/box2d.h>

class Minecart {
public:

	Minecart();
	void update();
	std::shared_ptr<Quad> quad;

	bool isAirborn = true;

private:
	float yVelocity = 0;
	bool wasSpacePressed = false;

	b2Body* body = nullptr;

	float baseVelocity = 5;
	float baseAccelerationWhenSlow = 200;
	float jumpForce = 8;
	float rotationalAcceleration = 1;
};