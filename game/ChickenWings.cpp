#include "ChickenWings.h"

ChickenWings ChickenWings::game("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
}

void ChickenWings::StartUp() {
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	groundCollider = world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundCollider->CreateFixture(&groundBox, 0.0f);

	minecart = std::make_unique<Minecart>();

	wireframe = false;
	spline = std::make_shared<Spline>();
	for (int32_t i = -9; i < 11; i++)
	{
		spline->addNextPoint({ i,  5  * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))});
	}
	splineRenderer = std::make_unique<SplineRenderer>(spline);
	splineRenderer->ybaseLine = -0.8f;
}

void ChickenWings::Update()
{
	minecart->update();
	UberShader::cameraPosition.x += deltaTime();
}

void ChickenWings::RenderObjects()
{
	splineRenderer->Render();
}
