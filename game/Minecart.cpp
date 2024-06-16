#include "Minecart.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

Minecart::Minecart() {
	quad = ChickenWings::game.quadManager->CreateQuad();
	quad->shader = ChickenWings::game.shader;
}

void Minecart::update()
{
	std::cout << quad->position.x << " | " << quad->position.y << std::endl;

	if (!wasSpacePressed && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;
		yVelocity = 0.4f;
	}

	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;

	yVelocity -= 9.81f * 0.01f;

	quad->position.y += yVelocity;
	
	if (quad->position.y < 0) {
		quad->position.y = 0;
		yVelocity = 0;
	}

}
