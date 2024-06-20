#pragma once
#include "Quad.h"
#include <box2d/box2d.h>

class Coin {
public:
	Coin(float xPos);
	void Update();
	bool isMarkdForDeletion = false;
	~Coin();

	void onCollision(b2Contact* contact);

private:
	static std::shared_ptr<Texture> colorTex;
	static std::shared_ptr<Texture> normalTex;
	float coinTime = 0.f;
	std::shared_ptr<Quad> quad;
	b2Body* body;
	std::string* tag = new std::string("Coin");
};