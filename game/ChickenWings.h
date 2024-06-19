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
