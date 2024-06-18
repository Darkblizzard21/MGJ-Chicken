#include "ChickenWings.h"
#include <iostream>

ChickenWings ChickenWings::game("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
}

void ChickenWings::StartUp() {

	minecart = std::make_unique<Minecart>();
	spline = std::make_shared<Spline>();
	for (int32_t i = -44; i < 23; i++)
	{
		spline->addNextPoint({ i*2,  3  * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))});
	}
	splineRenderer = std::make_unique<SplineRenderer>(spline);
	splineRenderer->ybaseLine = -0.8f;

	quad = quadManager.CreateQuad();
}

void ChickenWings::Update()
{
	minecart->update();
 	//UberShader::cameraPosition.x += deltaTime();
	UberShader::cameraPosition.x = minecart->quad->position.x;
	xPos += deltaTime();
	//quad->position = glm::vec2(xPos, spline->sampleHight(xPos));
	
}

void ChickenWings::RenderObjects()
{
	splineRenderer->Render();
}
