#include "ChickenWings.h"

ChickenWings ChickenWings::game ("ChickenWings");

ChickenWings::ChickenWings(std::string name) : App(name)
{
}

void ChickenWings::Update()
{
	minecart.update();
}
