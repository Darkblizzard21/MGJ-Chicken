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

	contactListener = std::make_unique<ContactListener>(*minecart, obstacles, coins);
	world.SetContactListener(contactListener.get());

	gameOverQuad = uiManager.CreateQuad(std::make_shared < Texture>("GameOver.png"));
	gameOverQuad->draw = false;
	gameOverQuad->scale = glm::vec2(10, 3);
	gameOverQuad->layer = 200;

	backflipQuad = uiManager.CreateQuad(std::make_shared < Texture>("backflip.png"));
	backflipQuad->draw = false;
	backflipQuad->layer = 200;

	frontflipQuad = uiManager.CreateQuad(std::make_shared < Texture>("frontflip.png"));
	frontflipQuad->draw = false;
	frontflipQuad->layer = 200;

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
		lanterns[i].quad->layer = 5;


		lanterns[i].light = compositPass_.CreatePointLight();
		lanterns[i].light->lightPos = lanterns[i].quad->position;
		lanterns[i].color = colors[i];
		lanterns[i].light->lightColor =  lanternStartColor * 0.8f;
		lanterns[i].light->lightRadius = 3;
	}

	coinIcon = uiManager.CreateQuad(std::make_shared<Texture>("chickencoin.png"));
	scoreNumberR = numberManager.CreateNumber();
	SetScorePosAndScale({ 7.5,4 }, 0.66f, false);
	constexpr float fadeInEndScale = 1.5f;
	fadeInAnimSpline.addNextPoint({ -0.2,0 });
	fadeInAnimSpline.addNextPoint({ -0.1,0 });
	fadeInAnimSpline.addNextPoint({ 0,0 });
	fadeInAnimSpline.addNextPoint({ 0.6, fadeInEndScale });
	fadeInAnimSpline.addNextPoint({ 1.f, fadeInEndScale });
	fadeInAnimSpline.addNextPoint({ 1.1, fadeInEndScale });
	fadeInAnimSpline.addNextPoint({ 1.2, fadeInEndScale });

	obstacles.push_back(std::make_unique<Obstacle>(-10));
	coins.push_back(std::make_unique<Coin>(-10));
}

void ChickenWings::ResetGame() {
	gameOverQuad->draw = false;
	isGameOver = false;
	meterScore = 0;
	bonusScore = 0;

	for (size_t i = 0; i < 8; i++)
	{
		lanterns[i].quad->position.x = lanternStep * -1.5f + lanternStep * i;
		lanterns[i].quad->position.y = spline->sampleHight(lanterns[i].quad->position.x) + 1.2f;
		lanterns[i].light->lightPos = lanterns[i].quad->position;
		lanterns[i].light->lightColor = lanternStartColor * 0.8f;
		lanterns[i].l = 0.f;
	}
	lastLantern = 0;

	SetScorePosAndScale({ 7.5,4 }, 0.66f, false);
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
			obstacles.erase(obstacles.begin() + i);
		}

	}

	for (int i = coins.size() - 1; i >= 0; i--)
	{
		coins[i]->Update();
	}

	timeUntilNextCoin -= deltaTime();
	if (timeUntilNextCoin <= 0) {
		timeUntilNextCoin = 12;
		coins.push_back(std::make_unique<Coin>(UberShader::cameraPosition.x + 10));

		if (coins.size() > 4) {
			coins.erase(coins.begin());
		}
	}

	for (int i = coins.size() - 1; i >= 0; i--)
	{
		if (coins[i]->isMarkdForDeletion) {
			coins.erase(coins.begin() + i);
		}

	}

	xPos += deltaTime();
	//quad->position = glm::vec2(xPos, spline->sampleHight(xPos));

	while (lanterns[lastLantern].quad->position.x - UberShader::cameraPosition.x < -14)
	{
		if (lanterns[lastLantern].l < 1)
			lanterns[lastLantern].l += 0.1f;
		else
			lanterns[lastLantern].l = 1;

		lanterns[lastLantern].quad->position.x = lanterns[(lastLantern + 7) % 8].quad->position.x + lanternStep;
		lanterns[lastLantern].quad->position.y = spline->sampleHight(lanterns[lastLantern].quad->position.x) + 0.7f;
		lanterns[lastLantern].light->lightPos = lanterns[lastLantern].quad->position;
		lanterns[lastLantern].light->lightColor = glm::mix(lanternStartColor, lanterns[lastLantern].color, lanterns[lastLantern].l) * 0.8f;
		lastLantern = (lastLantern + 1) % 8;
	}



	if (isGameOver) {
		AnimateGameOver();
		gameOverTime += deltaTime();

		if (glfwGetKey(ChickenWings::game.window, GLFW_KEY_SPACE) && gameOverTime > 0.75) {
			ResetGame();
		}
	}
	else {
		meterScore = glm::max(meterScore, (uint32_t)(minecart->quad->position.x * 12.f));
		scoreNumberR->SetNumber(Score());
	}

	UpdateFlipAnimation();

	minecart->updateAnimation();
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
	if (isGameOver != true) {
		gameOverTime = 0.f;
	}
	isGameOver = true;
}

void ChickenWings::ScoreCoin() {
	bonusScore += 300;
}

void ChickenWings::ShowFrontflip() {
	frontflipQuad->scale = { 0, 0 };
	frontflipQuad->draw = true;
	flipAnimationProgress = 0;
	isShowingFrontflip = true;
	bonusScore += 1000;
}

void ChickenWings::ShowBackflip() {
	backflipQuad->scale = { 0, 0 };
	backflipQuad->draw = true;
	flipAnimationProgress = 0;
	isShowingBackflip = true;
	bonusScore += 200;
}

void ChickenWings::UpdateFlipAnimation() {
	if (isShowingBackflip) {
		flipAnimationProgress += deltaTime() / flipAnimationDuration;
		if (flipAnimationProgress >= 1) {
			isShowingBackflip = false;
			backflipQuad->draw = false;
			return;
		}

		float scaleFactor = fadeInAnimSpline.sampleHight(flipAnimationProgress);

		backflipQuad->position = { 0, 1.5f };
		backflipQuad->scale = { 4 * scaleFactor, 4 * scaleFactor };
	}

	if (isShowingFrontflip) {
		flipAnimationProgress += deltaTime() / flipAnimationDuration;
		if (flipAnimationProgress >= 1) {
			isShowingFrontflip = false;
			frontflipQuad->draw = false;
			return;
		}

		float scaleFactor = fadeInAnimSpline.sampleHight(flipAnimationProgress);

		frontflipQuad->position = { 0, 1.5f };
		frontflipQuad->scale = { 4 * scaleFactor, 4 * scaleFactor };
	}
}

void ChickenWings::AnimateGameOver()
{
	gameOverQuad->draw = true;

	constexpr float screenAnimTime = 0.5f;
	if (gameOverTime < screenAnimTime) {
		float t = gameOverTime / screenAnimTime;
		gameOverQuad->scale = glm::vec2(10, 3) * glm::smoothstep(0.2f, 1.f, t);
	}
	else {
		gameOverQuad->scale = glm::vec2(10, 3);
	}
	constexpr float scoreFadeOutTime = 0.4f;
	constexpr float scoreFadeInTime = 0.4;
	if (gameOverTime < scoreFadeOutTime) {
		float t = gameOverTime / scoreFadeOutTime;
		float bonusX = (scoreNumberR->GetWidth() + 2.f) *  glm::smoothstep(0.f, 1.f, t);
		SetScorePosAndScale({ 7.5 + bonusX,4 }, 0.66f, false);
	}
	else if (gameOverTime < scoreFadeInTime + scoreFadeOutTime) {
		float t = (gameOverTime - scoreFadeOutTime) / scoreFadeInTime;
		float s = fadeInAnimSpline.sampleHight(t);
		SetScorePosAndScale({ 0, -1.5f }, fadeInAnimSpline.sampleHight(t), true);
	}
	else {
		SetScorePosAndScale({ 0, -1.5f }, 1.5f, true);
	}
}

uint32_t ChickenWings::Score()
{
	return meterScore + bonusScore;
}

void ChickenWings::SetScorePosAndScale(const glm::vec2& pos, const float& scale, bool centered)
{
	constexpr float padding = 0.8f;
	coinIcon->scale = glm::vec2(scale);
	coinIcon->position = pos;
	if (centered) {
		coinIcon->position.x += padding * 0.5f * scale * glm::floor(std::log10(Score()) + 1);
	}
	scoreNumberR->SetPos(pos - glm::vec2(0.74, 0));
	scoreNumberR->SetScale(coinIcon->scale);
	scoreNumberR->SetPadding(padding);
	scoreNumberR->SetCentered(centered);
}
