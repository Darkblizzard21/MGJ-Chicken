#include "Minecart.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "ChickenWings.h"

void Minecart::update()
{
	std::cout << position.x << " | " << position.y << std::endl;

	if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
		yVelocity = 5;
	}

	yVelocity -= 9.81f * 0.01f;

	position.y += yVelocity;

	if (position.y < 0) {
		position.y = 0;
		yVelocity = 0;
	}

}
