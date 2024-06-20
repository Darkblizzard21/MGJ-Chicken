#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include <box2d/box2d.h>
#include "Chicken.h"

class Minecart {
public:

	Minecart();
	void update();
	void updateAnimation();
	void onCollision(b2Contact* contact);
	std::shared_ptr<Quad> quad;
	std::shared_ptr<Quad> wheelL;
	std::shared_ptr<Quad> wheelR;

	bool isAirborn = true;
	b2Body* body = nullptr;

	float GetVelocityX() const { return velocity.x; }
	void reset();

private:
	float lastX = 0;
	float wheelRotation = 0.f;
	glm::vec2 velocity;
	bool wasSpacePressed = false;
	bool collisionExecutedThisFrame = false;

	float rotationDuringJump;
	float lastFrameRotation;
	bool hasDoneFlip;

	float baseVelocity = 5;
	float baseAccelerationWhenSlow = 5000;
	float jumpForce = 8;
	float rotationalAcceleration = 1;

	std::vector<std::unique_ptr<Chicken>> chickens;

	static std::shared_ptr<Texture> colorTex;
	static std::shared_ptr<Texture> normalTex;

	static std::shared_ptr<Texture> wheelColorTex;
	static std::shared_ptr<Texture> wheelNormalTex;
};