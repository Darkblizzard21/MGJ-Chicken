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
	for (int32_t i = -4; i < 15; i++)
	{
		spline->addNextPoint({ i* splineLeanth,  (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight});
		slopehight += slopehightIncrement;
		splineRendererB = std::make_unique<SplineRenderer>(spline);
		splineRendererB->ybaseLine = -10.8f - slopehight;
		splineSegmentCounter = i;

	}

	std::vector<glm::vec3> ground = { glm::vec3(74 / 255.f, 39 / 255.f, 9 / 255.f) };
	splineRendererB->texture = std::make_shared<Texture>(ground, 1, 1, SamplerTypes::NearestNeighbour);

	splineRendererL = std::make_unique<SplineRenderer>(spline, SplineMode::Line);
	splineRendererL->upperWidth = 0.0f;
	splineRendererL->layer++;
	std::vector<glm::vec3> metal = { glm::vec3(0.75, 0.75, 0.75) };
	splineRendererL->texture = std::make_shared<Texture>(metal, 1, 1, SamplerTypes::NearestNeighbour);


	// create lanterns
	auto lanternTex = std::make_shared<Texture>("lantern.png");
	std::vector<glm::vec3> colors = {
		 glm::vec3(0.33, 0.8, 0.33),
		 glm::vec3(0.630, 0.00, 0.00),
		 glm::vec3(0.386, 0.724, 0.920),
		 glm::vec3(0.930, 0.699, 0.0651),
		 glm::vec3(0.584, 0.950, 0.551),
		 glm::vec3(0.830, 0.461, 0.116),
		 glm::vec3(0.515, 0.539, 0.990),
		 glm::vec3(0.980, 0.323, 0.860)
	};
	for (size_t i = 0; i < 8; i++)
	{
		lanterns[i].quad = quadManager.CreateQuad(lanternTex);
		lanterns[i].quad->position.x = lanternStep * -1.5f + lanternStep * i;
		lanterns[i].quad->position.y = spline->sampleHight(lanterns[i].quad->position.x) + 1.2f;
		lanterns[i].quad->scale = glm::vec2(0.5f);


		lanterns[i].light = compositPass_.CreatePointLight();
		lanterns[i].light->lightPos = lanterns[i].quad->position;
		lanterns[i].light->lightColor = colors[i];
		lanterns[i].light->lightRadius = 3;
	}

	testNum = numberManager.CreateNumber();
	testNum->SetPos({ -5,4 });
}

void ChickenWings::Update()
{
	minecart->update();
	//UberShader::cameraPosition.x += deltaTime();
	float lerp = glm::min(1.f, minecart->GetVelocityX() / maxOffestVelo);
	float targetOffset = maxOffset * lerp * lerp;
	cameraOffset = glm::mix(cameraOffset, targetOffset, std::min(deltaTime(),1.f));
	UberShader::cameraPosition.x = minecart->quad->position.x + cameraOffset;
	UberShader::cameraPosition.y = minecart->quad->position.y;

	glm::vec2 v = spline->splinePoints[spline->splinePoints.size() - 1];
	float mincartDistanceToLastSplinePoint = glm::distance(v, minecart->quad->position);

	if (mincartDistanceToLastSplinePoint < glm::max(25.f, 10.f * lanternStep)) {
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

	while (lanterns[lastLantern].quad->position.x - UberShader::cameraPosition.x < -12)
	{
		lanterns[lastLantern].quad->position.x = lanterns[(lastLantern + 7) % 8].quad->position.x + lanternStep;
		lanterns[lastLantern].quad->position.y = spline->sampleHight(lanterns[lastLantern].quad->position.x) + 0.7f;
		lanterns[lastLantern].light->lightPos = lanterns[lastLantern].quad->position;
		lastLantern = (lastLantern + 1) % 8;
	}

	testNum->SetNumber((uint32_t)gameTime());
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

void ChickenWings::StopGame() {
	std::cout << "Game Over!" << std::endl;
}
