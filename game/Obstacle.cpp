#include "Obstacle.h"
#include "ChickenWings.h"

Obstacle::Obstacle(float xPos) {
	float yPos = ChickenWings::game.spline->sampleHight(xPos);

	quad = ChickenWings::game.quadManager.CreateQuad();
	quad->position = glm::vec2(xPos, yPos + 0.5f);
	quad->layer = 200;

	b2BodyDef bodyDef;
	bodyDef.position.Set(xPos, yPos + 0.5f);
	b2PolygonShape collider;
	collider.SetAsBox(0.5f, 0.5f);

	body = ChickenWings::game.world.CreateBody(&bodyDef);
	body->CreateFixture(&collider, 0);
}

Obstacle::~Obstacle() {
	ChickenWings::game.world.DestroyBody(body);
}