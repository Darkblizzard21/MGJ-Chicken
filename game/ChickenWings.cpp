#include "ChickenWings.h"

ChickenWings ChickenWings::game ("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"uniform vec2  pos; \n"
		"uniform vec2  scale; \n"
		"uniform float rotation; \n"
		"void main()\n"
		"{\n"
		"   float rotX = cos(rotation) * aPos.x - sin(rotation) * aPos.y;\n"
		"   float rotY = sin(rotation) * aPos.x + cos(rotation) * aPos.y;\n"
		"	vec2 wPos = vec2(rotX, rotY) * scale + pos;\n"
		"   gl_Position = vec4(wPos.x, wPos.y, 0.0f, 1.0);\n"
		"}\0";
	const std::string fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
	minecart = std::make_unique<Minecart>();
}

void ChickenWings::Update()
{
	minecart->update();
}
