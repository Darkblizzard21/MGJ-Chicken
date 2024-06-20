#pragma once
#include <box2d/box2d.h>
#include "Quad.h"
#include <memory>

class Chicken {
public:
	Chicken(b2Body* cartBody, float xOffset, float yOffset);
	~Chicken();
	void update();

	b2Body* body;
private:
	std::shared_ptr<Quad> quad;

	std::string* tag;
	
	b2WeldJoint* weldJoint;
};