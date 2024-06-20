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

	std::shared_ptr<Spline> spline;
	
private:
	std::unique_ptr<Minecart> minecart;
	std::queue<std::unique_ptr<Obstacle>> obstacles;
	float timeUntilNextObstacle = 5;

	std::unique_ptr<SplineRenderer> splineRendererB;
	std::unique_ptr<SplineRenderer> splineRendererL;

	float slopehight = 0;
	float timeToNextExpansion = 0;
	int splineSegmentCounter = -4;
	std::unique_ptr<ContactListener> contactListener;
};
