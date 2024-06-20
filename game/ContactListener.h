#pragma once
#include <box2d/box2d.h>
#include <queue>
#include "Minecart.h"
#include "Obstacle.h"

class ContactListener : public b2ContactListener {
public:
	ContactListener(Minecart& minecart, std::vector<std::unique_ptr<Obstacle>>& obsticles);

	virtual void BeginContact(b2Contact* contact) override;

private:
	Minecart& minecart;
	std::vector<std::unique_ptr<Obstacle>>& obsticles;
};