#include "ChickenWings.h"

ChickenWings ChickenWings::game ("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
	minecart = std::make_unique<Minecart>();

	wireframe = false;
	spline = std::make_shared<Spline>();
	spline->addNextPoint({ -1.25 ,0.5 });
	spline->addNextPoint({ -1.00 ,0.9 });
	spline->addNextPoint({ -0.75 ,1.0 });
	spline->addNextPoint({ -0.50 ,0.3 });
	spline->addNextPoint({ -0.25 ,0.5 });
	spline->addNextPoint({  0.00 ,0.0 });
	spline->addNextPoint({  0.25 ,0.7 });
	spline->addNextPoint({  0.50 ,0.6 });
	spline->addNextPoint({  0.75 ,0.3 });
	spline->addNextPoint({  1.00 ,0.2 });
	spline->addNextPoint({  1.25 ,0.5 });
	splineRenderer = std::make_unique<SplineRenderer>(spline);
	splineRenderer->sampleDensity = 33;
	splineRenderer->ybaseLine = -0.8;
}

void ChickenWings::Update()
{
	minecart->update();
}

void ChickenWings::RenderObjects()
{
	splineRenderer->Render();
}
