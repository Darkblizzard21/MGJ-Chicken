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
	collider.SetAsBox(0.25f, 0.25f);
	body->CreateFixture(&collider, 1.0f);

	// distance joint
	//b2DistanceJointDef distanceJointDef;
	//distanceJointDef.Initialize(cartBody, body, cartBody->GetPosition(), body-//>GetPosition());
	//distanceJointDef.collideConnected = false;
	//
	//float frequencyHz = 4.0f;
	//float dampingRatio = 0.5f;
	//b2LinearStiffness(distanceJointDef.stiffness, distanceJointDef.damping, frequencyHz, dampingRatio, distanceJointDef.bodyA, distanceJointDef.bodyB);

	//distanceJoint = (b2DistanceJoint*)ChickenWings::game.world.CreateJoint(&distanceJointDef);

	// revolute joint
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.Initialize(cartBody, body, cartBody->GetPosition());
	revoluteJointDef.lowerAngle = -0.2f * b2_pi; // pi = 180 d
	revoluteJointDef.upperAngle = 0.2f * b2_pi;
	revoluteJointDef.enableLimit = true;
	revoluteJointDef.maxMotorTorque = 10.0f;
	revoluteJointDef.motorSpeed = 0.0f;
	revoluteJointDef.enableMotor = true;

	revoluteJoint = (b2RevoluteJoint*)ChickenWings::game.world.CreateJoint(&revoluteJointDef);
}

void Chicken::update() {
	b2Vec2 pos = body->GetPosition();
	quad->position = glm::vec2(pos.x, pos.y);
	quad->rotation = body->GetAngle();

	// rotate upwards
	float angleError = revoluteJoint->GetJointAngle();
	float gain = 1.0f;
	revoluteJoint->SetMotorSpeed(-gain * angleError);
}