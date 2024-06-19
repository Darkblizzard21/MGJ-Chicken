#pragma once
#include <memory>
#include "Quad.h"
#include <box2d/box2d.h>

class Obstacle {
public:
	Obstacle(float xPos);
	~Obstacle();

private:
	std::shared_ptr<Quad> quad;
	b2Body* body;
};