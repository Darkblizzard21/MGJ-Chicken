#pragma once
#include <list>
#include <glm/glm.hpp>
#include <vector>

class Spline {

public:
	Spline();

	float sampleHight(float x);
	void addNextPoint(glm::vec2 p);
	glm::vec2 getPoint(float t);
	glm::vec2 getPoint(float t, int n);

	std::vector<glm::vec2> splinePoints;


};