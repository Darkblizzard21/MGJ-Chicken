#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>
#include "Spline.h"
#include "SplineRenderer.h"
#include "ContactListener.h"

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void StartUp() override;
	virtual void Update() override;
	virtual void RenderObjects() override;

private:
	std::unique_ptr<Minecart> minecart;
	std::shared_ptr<Spline> spline;
	std::unique_ptr<SplineRenderer> splineRendererB;
	std::unique_ptr<SplineRenderer> splineRendererL;

	std::shared_ptr<Quad> quad;

	std::shared_ptr<Quad> lantern;
	std::shared_ptr<PointLight> lanternL;

	float xPos = -8;
	float slopehight = 0;
	float timeToNextExpansion = 0;
	int splineSegmentCounter = -4;
	std::unique_ptr<ContactListener> contactListener;
};
