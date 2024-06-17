#pragma once
#include "Spline.h"

Spline::Spline()
{

}

float Spline::sampleHight(float x, int n)
{
	float currentT = 0.5;
	float currentSetp = 0.5;

	for (int i = 0; i < 10; i++) 
	{
		if (x < getPoint(currentT, n).x) {
			currentSetp = currentSetp / 2;
			currentT -= currentSetp;
		}
		else if (getPoint(currentT, n).x < x)
		{
			currentSetp = currentSetp / 2;
			currentT += currentSetp;
		}
		else 
		{
			break;
		}
	}

	return getPoint(currentT, n).y;
}

void Spline::addNextPoint(glm::vec2 p)
{
	splinePoints.push_back(p);
}

glm::vec2 Spline::getPoint(float t)
{
	return glm::vec2();
}

glm::vec2 Spline::getPoint(float t, int n)
{
	float x = 0;
	float y = 0;
	glm::vec2 p1 = splinePoints[n];
	glm::vec2 p2 = splinePoints[n + 1];
	glm::vec2 v1 = splinePoints[n + 1] - splinePoints[n - 1];
	glm::vec2 v2 = splinePoints[n + 2] - splinePoints[n];

	float t2 = t * t;
	float t3 = t2 * t;

	float h00 = 2 * t3 - 3 * t2 + 1;
	float h10 = t3 - 2 * t2 + t;
	float h01 = -2 * t3 + 3 * t2;
	float h11 = t3 - t2;

	x = h00 * p1.x + h10 * v1.x + h01 * p2.x + h11 * v2.x;
	y = h00 * p1.y + h10 * v1.y + h01 * p2.y + h11 * v2.y;

	return glm::vec2(x,y);
}
