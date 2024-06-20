#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include <box2d/box2d.h>
#include "Chicken.h"

class Minecart {
public:

	Minecart();
	void update();
	void onCollision(b2Contact* contact);
	std::shared_ptr<Quad> quad;

	bool isAirborn = true;

private:
	float yVelocity = 0;
	bool wasSpacePressed = false;
	bool collisionExecutedThisFrame = false;

	b2Body* body = nullptr;

	float baseVelocity = 5;
	float baseAccelerationWhenSlow = 500;
	float jumpForce = 8;
	float rotationalAcceleration = 1;

	std::vector<std::unique_ptr<Chicken>> chickens;
};