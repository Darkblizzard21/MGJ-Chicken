#include "Minecart.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

Minecart::Minecart() {
	quad = ChickenWings::game.quadManager.CreateQuad();
}

void Minecart::update()
{
	std::cout << quad->position.x << " | " << quad->position.y << std::endl;

	if (!wasSpacePressed && glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		wasSpacePressed = true;
		yVelocity = 5.0f;
	}

	if (!glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE))
		wasSpacePressed = false;

	yVelocity -= 9.81f * ChickenWings::game.deltaTime();

	//quad->rotation += ChickenWings::game.deltaTime() * 0.001f;
	quad->scale.x = 9 / 16.0f;

	quad->position.y += yVelocity * ChickenWings::game.deltaTime();
	
	if (quad->position.y < 0) {
		quad->position.y = 0;
		yVelocity = 0;
	}

}
