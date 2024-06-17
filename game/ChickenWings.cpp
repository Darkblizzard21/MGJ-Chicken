#include "ChickenWings.h"

ChickenWings ChickenWings::game ("ChickenWings");


ChickenWings::ChickenWings(std::string name)
	: App(name)
{
	minecart = std::make_unique<Minecart>();
}

void ChickenWings::Update()
{
	minecart->update();
}
