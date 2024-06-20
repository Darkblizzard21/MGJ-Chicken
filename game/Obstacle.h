#pragma once
#include <memory>
#include "Quad.h"
#include <box2d/box2d.h>
#include "DeferredComposit.h"

class Obstacle {
public:
	Obstacle(float xPos);
	void Update();
	bool isMarkdForDeletion = false;
	~Obstacle();

	void onCollision(b2Contact* contact);

private:
	static std::shared_ptr<Texture> colorTex;
	static std::shared_ptr<Texture> normalTex;
	std::shared_ptr<Quad> quad;
	b2Body* body;
	std::string* tag = new std::string("Obstacle");
	std::shared_ptr<PointLight> light;

};