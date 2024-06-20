#include "Minecart.h"
#include <GLFW/glfw3.h>
#include "ChickenWings.h"
#include "MathUtil.h"
#include <iostream>

std::shared_ptr<Texture> Minecart::colorTex = nullptr;
std::shared_ptr<Texture> Minecart::normalTex = nullptr;
std::shared_ptr<Texture> Minecart::wheelColorTex = nullptr;
std::shared_ptr<Texture> Minecart::wheelNormalTex = nullptr;

Minecart::Minecart() {
	if (colorTex == nullptr) colorTex = std::make_shared<Texture>("Cart.png");
	if (normalTex == nullptr) normalTex = std::make_shared<Texture>("CartN.png");
	if (wheelColorTex == nullptr) wheelColorTex = std::make_shared<Texture>("wheel.png");
	if (wheelNormalTex == nullptr) wheelNormalTex = std::make_shared<Texture>("wheelN.png");

	quad = ChickenWings::game.quadManager.CreateQuad(colorTex, normalTex);
	quad->layer = 200;

	wheelL = ChickenWings::game.quadManager.CreateQuad(wheelColorTex, wheelNormalTex);
	wheelL->layer = 205;
	wheelL->scale = { 0.33f, 0.33f };
	wheelR = ChickenWings::game.quadManager.CreateQuad(wheelColorTex, wheelNormalTex);
	wheelR->layer = 205;
	wheelR->scale = { 0.33f, 0.33f };

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 0.0f);
	body = ChickenWings::game.world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.4f, 0.3f);
	b2FixtureDef boxFixture;
	boxFixture.shape = &dynamicBox;
	boxFixture.density = 1.0f;
	boxFixture.friction = 2.5f;


	float density = 5.0f;
	float friction = 0.1f;

	b2CircleShape circleShape1;
	circleShape1.m_radius = 0.15f;
	circleShape1.m_p = b2Vec2(-0.3f, -0.2f);

	b2FixtureDef wheelFixture1;
	wheelFixture1.shape = &circleShape1;
	wheelFixture1.density = density;
	wheelFixture1.friction = friction;


	b2CircleShape circleShape2;
	circleShape2.m_radius = 0.15f;
	circleShape2.m_p = b2Vec2(0.3f, -0.2f);

	b2FixtureDef wheelFixture2;
	wheelFixture2.shape = &circleShape2;
	wheelFixture2.density = density;
	wheelFixture2.friction = friction;

	body->CreateFixture(&boxFixture);
	body->CreateFixture(&wheelFixture1);
	body->CreateFixture(&wheelFixture2);

	body->SetAngularDamping(3.0f);

	chickens.push_back(std::make_unique<Chicken>(body, 0.35f, 0));
	chickens.push_back(std::make_unique<Chicken>(body, 0.05f, -0.03f));
	chickens.push_back(std::make_unique<Chicken>(body, -0.2f, 0.05f));
}

void Minecart::update()
{
	// sync
	b2Vec2 phyisicsPos = body->GetPosition();
	quad->position = glm::vec2(phyisicsPos.x, phyisicsPos.y);
	quad->rotation = body->GetAngle();


	for (size_t i = 0; i < chickens.size(); i++)
	{
		chickens[i]->update();
	}

	auto v = body->GetLinearVelocity();
	velocity = glm::vec2(v.x, v.y);

	collisionExecutedThisFrame = false;

	if (isAirborn && !hasDoneFlip) {
		float deltaAngle = body->GetAngle() - lastFrameRotation;
		rotationDuringJump += deltaAngle;
		lastFrameRotation = body->GetAngle();

		// +/-1 is threshold reduction to make it easier
		if (rotationDuringJump > 2 * b2_pi - 1) {
			ChickenWings::game.ShowBackflip();
			hasDoneFlip = true;
		}
		else if (rotationDuringJump < -2 * b2_pi + 1) {
			ChickenWings::game.ShowFrontflip();
			hasDoneFlip = true;
		}
	}

	if (ChickenWings::game.isGameOver)
		return;

	if (!isAirborn && body->GetLinearVelocity().Length() < baseVelocity) {
		body->ApplyForceToCenter(b2Vec2(baseAccelerationWhenSlow * ChickenWings::game.deltaTime(), 0), true);
	}

	// jumping
	if (!wasSpacePressed && !isAirborn && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;

		b2Vec2 forceVector = b2Vec2(0, jumpForce);
		body->ApplyLinearImpulseToCenter(forceVector, true);

		rotationDuringJump = 0;
		lastFrameRotation = body->GetAngle();

		isAirborn = true;
	}

	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;

	if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_A)) {
		body->ApplyTorque(rotationalAcceleration, true);
	}
	if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_D)) {
		body->ApplyTorque(-rotationalAcceleration, true);
	}
}

void Minecart::updateAnimation()
{
	// leftWeel
	wheelL->position = quad->position + rotate(glm::vec2(-0.25, -0.25), quad->rotation);
	wheelR->position = quad->position + rotate(glm::vec2(0.25, -0.25), quad->rotation);

	const float deltaX = quad->position.x - lastX;
	lastX = quad->position.x;
	float rotThisFrame = deltaX / (wheelL->scale.x * 3.141592);
	if (isAirborn) {
		rotThisFrame /= 2;
	}
	wheelRotation += rotThisFrame;
	if (wheelRotation > 2 * 3.141592) {
		wheelRotation -= 2 * 3.141592;
	}
	wheelL->rotation = quad->rotation - wheelRotation;
	wheelR->rotation = quad->rotation - wheelRotation;
}

void Minecart::onCollision(b2Contact* contact) {
	int chickenCounter = 0;
	int obstacleCounter = 0;
	uintptr_t ptrA = contact->GetFixtureA()->GetUserData().pointer;
	if (ptrA != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrA);
		if (tag == "Chicken")
			chickenCounter++;
		else if (tag == "Obstacle")
			obstacleCounter++;
	}

	uintptr_t ptrB = contact->GetFixtureB()->GetUserData().pointer;
	if (ptrB != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrB);
		if (tag == "Chicken")
			chickenCounter++;
		else if (tag == "Obstacle")
			obstacleCounter++;
	}

	if (chickenCounter == 1 && obstacleCounter == 0)
		ChickenWings::game.StopGame();

	if (collisionExecutedThisFrame)
		return;
	collisionExecutedThisFrame = true;

	hasDoneFlip = false;
	isAirborn = false;
}

void Minecart::reset() {
	body->SetLinearVelocity(b2Vec2_zero);
	body->SetAngularVelocity(0);
	body->SetTransform(b2Vec2(0, 0), 0);

	for (size_t i = 0; i < chickens.size(); i++)
	{
		chickens[i]->body->SetLinearVelocity(b2Vec2_zero);
		chickens[i]->body->SetAngularVelocity(0);
	}

	chickens[0]->body->SetTransform(body->GetPosition() + b2Vec2(0.35f, 0.35f), 0);
	chickens[1]->body->SetTransform(body->GetPosition() + b2Vec2(0.05f, 0.32f), 0);
	chickens[2]->body->SetTransform(body->GetPosition() + b2Vec2(-0.2f, 0.4f), 0);
}