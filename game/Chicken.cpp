#include "Chicken.h"
#include "ChickenWings.h"

Chicken::Chicken(b2Body* cartBody, float xOffset, float yOffset) {
	quad = ChickenWings::game.quadManager.CreateQuad(std::make_shared<Texture>("ChickenWhite_closed.png"));
	quad->scale = glm::vec2(0.5f, 0.5f);
	quad->layer = 100;

	b2BodyDef bodyDef;
	bodyDef.position = cartBody->GetPosition() + b2Vec2(xOffset, 0.35f + yOffset);
	bodyDef.type = b2_dynamicBody;

	body = ChickenWings::game.world.CreateBody(&bodyDef);

	b2FixtureDef fixtureDef;
	b2PolygonShape collider;
	collider.SetAsBox(0.05f, 0.2f);
	
	fixtureDef.shape = &collider;
	fixtureDef.density = 0.05f;

	tag = new std::string("Chicken");
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(tag);

	body->CreateFixture(&fixtureDef);

	b2WeldJointDef jointDef;
	jointDef.Initialize(cartBody, body, cartBody->GetPosition() + b2Vec2(xOffset + 0.15f, 0));
	jointDef.collideConnected = false;
	
	float frequencyHz = 2.0f;
	float dampingRatio = 0.3f;
	b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequencyHz, dampingRatio, jointDef.bodyA, jointDef.bodyB);

	weldJoint = (b2WeldJoint*)ChickenWings::game.world.CreateJoint(&jointDef);
}

Chicken::~Chicken()
{
	delete tag;
	ChickenWings::game.world.DestroyJoint(weldJoint);
}

void Chicken::update() {
	b2Vec2 pos = body->GetPosition();
	quad->position = glm::vec2(pos.x, pos.y);
	quad->rotation = body->GetAngle();

}