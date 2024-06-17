#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>

class ChickenWings : public App {
public:
	static ChickenWings game;

	ChickenWings(std::string name);
	virtual void StartUp() override;
	virtual void Update() override;

private:
	std::unique_ptr<Minecart> minecart;
	b2Body* groundCollider = nullptr;
};
