#include "Minecart.h"
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

Minecart::Minecart() {
	quad = ChickenWings::game.quadManager.CreateQuad(std::make_shared<Texture>("Minecart.png"), std::make_shared<Texture>("MinecartN.png"));

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

	chickens.push_back(std::make_unique<Chicken>(body));
}

void Minecart::update()
{
	// sync
	b2Vec2 phyisicsPos = body->GetPosition();
	quad->position = glm::vec2(phyisicsPos.x, phyisicsPos.y);
	quad->rotation = body->GetAngle();
	
	// jumping
	if (!wasSpacePressed && !isAirborn && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;

		// up vector
		//b2Vec2 forceVector(-glm::sin(quad->rotation), glm::cos(quad->rotation));
		b2Vec2 forceVector = b2Vec2(0, jumpForce);

		body->ApplyLinearImpulseToCenter(forceVector, true);
		//body->ApplyAngularImpulse(0.2f, true);

		isAirborn = true;
	}
	
	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;

	if (!isAirborn && body->GetLinearVelocity().Length() < baseVelocity) {
		body->ApplyForceToCenter(b2Vec2(baseAccelerationWhenSlow * ChickenWings::game.deltaTime(), 0), true);
	}

	if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_A)) {
		body->ApplyTorque(rotationalAcceleration, true);
	}
	if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_D)) {
		body->ApplyTorque(-rotationalAcceleration, true);
	}

	for (size_t i = 0; i < chickens.size(); i++)
	{
		chickens[i]->update();
	}
}

void Minecart::onCollision() {
	isAirborn = false;
}
