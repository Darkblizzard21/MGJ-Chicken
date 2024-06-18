#pragma once
#include <list>
#include <glm/glm.hpp>
#include <vector>
#include <box2d/box2d.h>

class Spline {

public:
	Spline();

	float sampleHight(float x, int n);
	float sampleHight(float x);
	float sampleT(float x, int n);
	float sampleT(float x);
	glm::vec2 sampleHightAndT(float x, int n);
	glm::vec2 sampleHightAndT(float x);

	void addNextPoint(glm::vec2 p);
	glm::vec2 getPoint(float t);
	glm::vec2 getPoint(float t, int n);
	glm::vec2 getTangent(float t, int n);
	glm::vec2 getNormal(float t, int n);

	float scale = 0.5f;
	std::vector<glm::vec2> splinePoints;

private:
	b2Body* collider = nullptr;
	std::vector<b2Fixture*> fixtures;
	const int collisionSamplesPerSegment = 10;
};