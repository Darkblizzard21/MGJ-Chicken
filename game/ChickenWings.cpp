#include "ChickenWings.h"
#include <iostream>

ChickenWings ChickenWings::game("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
}

void ChickenWings::StartUp() {
	minecart = std::make_unique<Minecart>();
	
	contactListener = std::make_unique<ContactListener>(*minecart);
	world.SetContactListener(contactListener.get());

	spline = std::make_shared<Spline>();
	for (int32_t i = -44; i < 43; i++)
	{
		spline->addNextPoint({ i*6,  2  * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))});
	}
	splineRendererB = std::make_unique<SplineRenderer>(spline);
	splineRendererB->ybaseLine = -0.8f;

	std::vector<glm::vec3> ground = { glm::vec3(74 / 255.f, 39 / 255.f, 9/255.f) };
	splineRendererB->texture = std::make_shared<Texture>(ground, 1, 1, SamplerTypes::NearestNeighbour);

	splineRendererL = std::make_unique<SplineRenderer>(spline, SplineMode::Line);
	splineRendererL->upperWidth = 0.f;
	splineRendererL->layer++;
	std::vector<glm::vec3> metal = { glm::vec3(0.75, 0.75, 0.75) };
	splineRendererL->texture = std::make_shared<Texture>(metal, 1, 1, SamplerTypes::NearestNeighbour);

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
	splineRendererB->Render();
	splineRendererL->Render();
}
