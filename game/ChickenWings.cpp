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
	for (int32_t i = -4; i < 5; i++)
	{
		spline->addNextPoint({ i,  5  * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))});
	}
	splineRenderer = std::make_unique<SplineRenderer>(spline);
	splineRenderer->ybaseLine = -0.8f;

	std::vector<b2Vec2> splineColliderPoints;
	for (int i = spline->splinePoints.size()-1; i >= 0 ; i--)
	{
		glm::vec2 point = spline->splinePoints[i];
		splineColliderPoints.push_back(b2Vec2(point.x, point.y));
	}
	b2ChainShape chain;
	chain.CreateLoop(&splineColliderPoints[0], splineColliderPoints.size());

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);

	groundCollider = world.CreateBody(&groundBodyDef);
	groundCollider->CreateFixture(&chain, 0.0f);

	quad = quadManager.CreateQuad();
}

void ChickenWings::Update()
{
	minecart->update();
 	//UberShader::cameraPosition.x += deltaTime();

	xPos += deltaTime();
	//quad->position = glm::vec2(xPos, spline->sampleHight(xPos));
	
}

void ChickenWings::RenderObjects()
{
	splineRenderer->Render();
}
