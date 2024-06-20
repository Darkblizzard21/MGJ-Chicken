#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include <box2d/box2d.h>
#include "Chicken.h"

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
	float baseAccelerationWhenSlow = 100;
	float jumpForce = 8;
	float rotationalAcceleration = 1;

	std::vector<std::unique_ptr<Chicken>> chickens;
};