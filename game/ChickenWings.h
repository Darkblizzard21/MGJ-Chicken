#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void Update() override;

private:
	std::unique_ptr<Minecart> minecart;
};
