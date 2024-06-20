#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>
#include "Spline.h"
#include "SplineRenderer.h"
#include "ContactListener.h"
#include "Obstacle.h"
#include <queue>

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void StartUp() override;
	virtual void Update() override;
	virtual void RenderObjects() override;
	void StopGame();

	std::shared_ptr<Spline> spline;

private:
	uint32_t Score();
	uint32_t bounsScore = 0;
	uint32_t meters = 0;

	glm::vec2 cameraOffset = { 0, 0 };
	float maxOffset = 6;
	float maxOffestVelo = 10.f;

	std::unique_ptr<Minecart> minecart;
	std::queue<std::unique_ptr<Obstacle>> obstacles;
	float timeUntilNextObstacle = 5;

	std::unique_ptr<SplineRenderer> splineRendererB;
	std::unique_ptr<SplineRenderer> splineRendererL;

	std::shared_ptr<Number> testNum;

	struct Lantern
	{

		std::shared_ptr<Quad> quad;
		std::shared_ptr<PointLight> light;
	};
	Lantern lanterns[8];
	int lastLantern = 0;
	float lanternStep = 5.f;

	float xPos = -8;
	float timeToNextExpansion = 0;
	std::unique_ptr<ContactListener> contactListener;

	//PCG
	int splineSegmentCounter = -4;
	float slopehight = 0;
	float slopehightIncrement = 1;

	float SplineXStep = 6;

	float splineLeanth = 6;

	float splineHightVariance = 2;
	int currentLevel = 1;

	void GenerateNextPointOnSpline();


};
