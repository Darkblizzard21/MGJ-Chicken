#include "Obstacle.h"
#include "ChickenWings.h"

Obstacle::Obstacle(float xPos) {
	float yPos = ChickenWings::game.spline->sampleHight(xPos);

	quad = ChickenWings::game.quadManager.CreateQuad(std::make_shared<Texture>("Shroom.png"));
	quad->position = glm::vec2(xPos, yPos + 0.42f);
	quad->layer = 50;

	b2BodyDef bodyDef;
	bodyDef.position.Set(xPos, yPos + 0.5f);
	b2PolygonShape collider;
	collider.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	fixtureDef.shape = &collider;
	fixtureDef.density = 0;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(tag);

	body = ChickenWings::game.world.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);

	light = ChickenWings::game.compositPass_.CreatePointLight();
	light->lightPos = quad->position;
	light->lightRadius = 1.5f;
	light->lightColor = glm::vec3(0.5f, 0.5f, 0.5f);
}

void Obstacle::Update()
{

}

Obstacle::~Obstacle() {
	delete tag;
	ChickenWings::game.world.DestroyBody(body);
}

void Obstacle::onCollision(b2Contact* contact)
{
	
}
