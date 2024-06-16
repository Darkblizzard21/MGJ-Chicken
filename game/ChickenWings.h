#pragma once
#include "App.h"
#include "Minecart.h"

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void Update() override;

private:
	Minecart minecart;
};
