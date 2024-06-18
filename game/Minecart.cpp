#include "Minecart.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

Minecart::Minecart() {
	quad = ChickenWings::game.quadManager.CreateQuad();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 8.0f);
	body = ChickenWings::game.world.CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);

}

void Minecart::update()
{
	b2Vec2 phyisicsPos = body->GetPosition();
	quad->position = glm::vec2(phyisicsPos.x, phyisicsPos.y);
	quad->rotation = body->GetAngle();
	
	// std::cout << quad->position.x << " | " << quad->position.y << std::endl;

	if (!wasSpacePressed && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;
		body->ApplyForceToCenter(b2Vec2(0, 500), true);
	}
	
	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;
}
