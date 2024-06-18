#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>
#include "Spline.h"
#include "SplineRenderer.h"

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void StartUp() override;
	virtual void Update() override;
	virtual void RenderObjects() override;

private:
	std::unique_ptr<Minecart> minecart;
	b2Body* groundCollider = nullptr;
	std::shared_ptr<Spline> spline;
	std::unique_ptr<SplineRenderer> splineRenderer;

	std::shared_ptr<Quad> quad;
	float xPos = -8;
};
