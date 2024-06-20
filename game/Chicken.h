#pragma once
#include <box2d/box2d.h>
#include "Quad.h"
#include <memory>

class Chicken {
public:
	Chicken(b2Body* cartBody);
	void update();

private:
	std::shared_ptr<Quad> quad;
	
	b2Body* body;
	b2DistanceJoint* distanceJoint;
	b2RevoluteJoint* revoluteJoint;
};