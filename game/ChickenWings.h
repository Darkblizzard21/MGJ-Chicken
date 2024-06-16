#pragma once
#include "App.h"
#include "Minecart.h"
#include <memory>

class ChickenWings : public App {
public:
	static ChickenWings game;
	std::shared_ptr<Shader> shader;

	ChickenWings(std::string name);
	virtual void Update() override;

private:
	std::unique_ptr<Minecart> minecart;
};
