#include "Minecart.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

Minecart::Minecart() {
	quad = ChickenWings::game.quadManager.CreateQuad(std::make_shared<Texture>("Minecart.png"), std::make_shared<Texture>("MinecartN.png"));

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 7.0f);
	body = ChickenWings::game.world.CreateBody(&bodyDef);

	float density = 1.0f;
	float friction = 0.3f;

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.4f, 0.3f);
	b2FixtureDef boxFixture;
	boxFixture.shape = &dynamicBox;
	boxFixture.density = density;
	boxFixture.friction = 2.5f;


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
}

void Minecart::update()
{
	b2Vec2 phyisicsPos = body->GetPosition();
	quad->position = glm::vec2(phyisicsPos.x, phyisicsPos.y);
	quad->rotation = body->GetAngle();

	// std::cout << quad->position.x << " | " << quad->position.y << std::endl;

	if (!wasSpacePressed && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;

		// up vector
		b2Vec2 forceVector(-glm::sin(quad->rotation), glm::cos(quad->rotation));
		forceVector *= 300;

		body->ApplyForceToCenter(forceVector, true);
		//body->ApplyAngularImpulse(0.2f, true);
	}

	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;
}
