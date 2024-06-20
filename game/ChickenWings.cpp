#include "ChickenWings.h"
#include <iostream>
#include <GLFW/glfw3.h>

ChickenWings ChickenWings::game("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
}

void ChickenWings::StartUp() {
	minecart = std::make_unique<Minecart>();

	contactListener = std::make_unique<ContactListener>(*minecart,obstacles);
	world.SetContactListener(contactListener.get());

	gameOverQuad = uiManager.CreateQuad(std::make_shared < Texture>("GameOver.png"));
	gameOverQuad->draw = false;
	gameOverQuad->scale = glm::vec2(10, 3);
	gameOverQuad->layer = 200;

	spline = std::make_shared<Spline>();
	for (int32_t i = -4; i < 15; i++)
	{
		spline->addNextPoint({ i * splineLeanth,  (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight });
		slopehight += slopehightIncrement;
		splineRendererB = std::make_unique<SplineRenderer>(spline);
		splineRendererB->ybaseLine = -10.8f - slopehight;
		splineSegmentCounter = i;

	}

	std::vector<glm::vec3> ground = { glm::vec3(74 / 255.f, 39 / 255.f, 9 / 255.f) };
	splineRendererB->texture = std::make_shared<Texture>("Ground.png", SamplerTypes::NearestNeighbour);
	splineRendererB->uvScale = glm::vec2(0.2f, 0.2f);

	splineRendererL = std::make_unique<SplineRenderer>(spline, SplineMode::Line);
	splineRendererL->upperWidth = 0.0f;
	splineRendererL->layer++;
	std::vector<glm::vec3> metal = { glm::vec3(0.5f, 0.5f, 0.5f) };
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

	coinIcon = uiManager.CreateQuad(std::make_shared<Texture>("chickencoin.png"));
	coinIcon->scale = { 0.66f, 0.66f };
	coinIcon->position = { 7.5,4 };
	scoreNumberR = numberManager.CreateNumber();
	scoreNumberR->SetPos(coinIcon->position - glm::vec2(0.74,0));
	scoreNumberR->SetScale(coinIcon->scale);
	scoreNumberR->SetPadding(0.8f);
}

void ChickenWings::ResetGame() {
	gameOverQuad->draw = false;
	isGameOver = false;
	meterScore = 0;
	bounsScore = 0;

	for (size_t i = 0; i < 8; i++)
	{
		lanterns[i].quad->position.x = lanternStep * -1.5f + lanternStep * i;
		lanterns[i].quad->position.y = spline->sampleHight(lanterns[i].quad->position.x) + 1.2f;
		lanterns[i].light->lightPos = lanterns[i].quad->position;
	}

	minecart->reset();
}

void ChickenWings::Update()
{
	minecart->update();
	float lerp = glm::min(1.f, minecart->GetVelocityX() / maxOffestVelo);
	float targetOffsetX = maxOffset * lerp * lerp;
	float targetOffsetY = spline->sampleHight(UberShader::cameraPosition.x + 8) - UberShader::cameraPosition.y + 4.5f;
	targetOffsetY = glm::clamp(targetOffsetY, -6.f, 0.f);
	cameraOffset = glm::mix(cameraOffset, { targetOffsetX, targetOffsetY }, std::min(deltaTime() * 2, 1.f));
	UberShader::cameraPosition = minecart->quad->position + cameraOffset;

	glm::vec2 v = spline->splinePoints[spline->splinePoints.size() - 1];
	float mincartDistanceToLastSplinePoint = glm::distance(v, minecart->quad->position);

	if (mincartDistanceToLastSplinePoint < glm::max(25.f, 10.f * lanternStep)) {
		GenerateNextPointOnSpline();
	}

	timeUntilNextObstacle -= deltaTime();
	if (timeUntilNextObstacle <= 0) {
		timeUntilNextObstacle = 5;
		obstacles.push_back(std::make_unique<Obstacle>(UberShader::cameraPosition.x + 10));

		if (obstacles.size() > 4) {
			obstacles.erase(obstacles.begin());
		}



	}

	for (int i = obstacles.size() - 1; i >= 0; i--)
	{
		if (obstacles[i]->isMarkdForDeletion) {
			std::cout << "Obsacle Deleated" << std::endl;

			obstacles.erase(obstacles.begin() + i);
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



	if (isGameOver) {
		gameOverQuad->draw = true;
		if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE)) {
			ResetGame();
		}
	}
	else {
		meterScore = glm::max(meterScore, (uint32_t)(minecart->quad->position.x * 4.f));
		scoreNumberR->SetNumber(Score());
	}
}

void ChickenWings::RenderObjects()
{
	splineRendererB->Render();
	splineRendererL->Render();
}

void ChickenWings::GenerateNextPointOnSpline()
{
	glm::vec2 v = spline->splinePoints[spline->splinePoints.size() - 1];

	spline->addNextPoint({ v.x + SplineXStep, (splineHightVariance * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) - slopehight });

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
	isGameOver = true;
}

uint32_t ChickenWings::Score()
{
	return meterScore + bounsScore;
}
