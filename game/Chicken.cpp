#include "Chicken.h"
#include "ChickenWings.h"

Chicken::Chicken(b2Body* cartBody) {
	quad = ChickenWings::game.quadManager.CreateQuad(std::make_shared<Texture>("ChickenWhite_closed.png"));
	quad->scale = glm::vec2(0.5f, 0.5f);
	quad->layer = 100;

	b2BodyDef bodyDef;
	bodyDef.position = cartBody->GetPosition() + b2Vec2(0, 0.35f);
	bodyDef.type = b2_dynamicBody;

	body = ChickenWings::game.world.CreateBody(&bodyDef);

	b2PolygonShape collider;
	collider.SetAsBox(0.2f, 0.2f);
	body->CreateFixture(&collider, 0.05f);

	b2WeldJointDef jointDef;
	jointDef.Initialize(cartBody, body, cartBody->GetPosition());
	
	float frequencyHz = 2.0f;
	float dampingRatio = 0.3f;
	b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequencyHz, dampingRatio, jointDef.bodyA, jointDef.bodyB);

	weldJoint = (b2WeldJoint*)ChickenWings::game.world.CreateJoint(&jointDef);
}

void Chicken::update() {
	b2Vec2 pos = body->GetPosition();
	quad->position = glm::vec2(pos.x, pos.y);
	quad->rotation = body->GetAngle();

}