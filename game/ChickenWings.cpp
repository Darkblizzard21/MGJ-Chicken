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
	for (int32_t i = -4; i < 10; i++)
	{
		spline->addNextPoint({ i* splineLeanth,  (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight});
		slopehight += slopehightIncrement;
		splineRendererB = std::make_unique<SplineRenderer>(spline);
		splineRendererB->ybaseLine = -10.8f - slopehight;
		splineSegmentCounter = i;

	}

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
	UberShader::cameraPosition.y = minecart->quad->position.y;

	glm::vec2 v = spline->splinePoints[spline->splinePoints.size() - 1];
	float mincartDistanceToLastSplinePoint = glm::distance(v, minecart->quad->position);

	if (mincartDistanceToLastSplinePoint < 25.0) {
		GenerateNextPointOnSpline();
	}

	timeUntilNextObstacle -= deltaTime();
	if (timeUntilNextObstacle <= 0) {
		timeUntilNextObstacle = 5;
		obstacles.push(std::make_unique<Obstacle>(UberShader::cameraPosition.x + 10));

		if (obstacles.size() > 4) {
			obstacles.pop();
		}
	}
	xPos += deltaTime();
	//quad->position = glm::vec2(xPos, spline->sampleHight(xPos));
	
}

void ChickenWings::RenderObjects()
{
	splineRendererB->Render();
	splineRendererL->Render();
}

void ChickenWings::GenerateNextPointOnSpline()
{
	glm::vec2 v = spline->splinePoints[spline->splinePoints.size() - 1];

	spline->addNextPoint({v.x + SplineXStep, (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight });

	//spline->addNextPoint({ splineSegmentCounter * splineLeanth,  (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight });
	slopehight += slopehightIncrement;
	splineSegmentCounter++;
	splineRendererB->ybaseLine = -10.8f - slopehight;

	if (splineSegmentCounter > 20 && splineSegmentCounter % 10 == 0)
	{
		splineHightVariance += 0.5;

		std::cout << "Next Level" << std::endl;
		if (slopehightIncrement < 2.5)
		{
			slopehightIncrement += 0.5;
		}
		if (SplineXStep < 9)
		{
			SplineXStep += 0.5;
		}
		if (splineHightVariance < 5)
		{
			splineHightVariance += 0.5;
			//splineHightVariance
		}

	}
}
