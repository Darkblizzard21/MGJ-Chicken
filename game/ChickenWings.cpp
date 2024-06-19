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
		spline->addNextPoint({ i*6,  (2  * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight});
		slopehight += 1;
		splineRendererB = std::make_unique<SplineRenderer>(spline);
		splineRendererB->ybaseLine = -10.8f - slopehight;
		splineSegmentCounter = i;

	}

	std::vector<glm::vec3> ground = { glm::vec3(74 / 255.f, 39 / 255.f, 9/255.f) };
	splineRendererB->texture = std::make_shared<Texture>(ground, 1, 1, SamplerTypes::NearestNeighbour);

	splineRendererL = std::make_unique<SplineRenderer>(spline, SplineMode::Line);
	splineRendererL->upperWidth = 0.0f;
	splineRendererL->layer++;
	std::vector<glm::vec3> metal = { glm::vec3(0.75, 0.75, 0.75) };
	splineRendererL->texture = std::make_shared<Texture>(metal, 1, 1, SamplerTypes::NearestNeighbour);

	quad = quadManager.CreateQuad();

	// create lanterns
	lantern = quadManager.CreateQuad(std::make_shared<Texture>("lantern.png"));
	lantern->position.y = spline->sampleHight(0) + 0.7f;
	lantern->scale = glm::vec2(0.5f);
	lanternL = compositPass_.CreatePointLight();
	lanternL->lightPos = lantern->position;
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
		spline->addNextPoint({ splineSegmentCounter * 6,  (2 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight });
		slopehight += 1;
		splineSegmentCounter++;
	}

	xPos += deltaTime();
	//quad->position = glm::vec2(xPos, spline->sampleHight(xPos));
	
	float lanterDelta = glm::abs(UberShader::cameraPosition.x - lantern->position.x);
	if (lanterDelta > 15) {
		lantern->position.x += 2 * lanterDelta;
		lantern->position.y = spline->sampleHight(lantern->position.x) + 1.7f; 
		lanternL->lightPos = lantern->position;
	}
}

void ChickenWings::RenderObjects()
{
	splineRendererB->Render();
	splineRendererL->Render();
}
