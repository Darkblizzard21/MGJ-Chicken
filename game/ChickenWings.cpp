#include "ChickenWings.h"

ChickenWings ChickenWings::game ("ChickenWings");


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

}

void ChickenWings::Update()
{
	minecart->update();
}
