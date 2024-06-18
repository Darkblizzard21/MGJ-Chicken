#pragma once
#include "Spline.h"
#include "ChickenWings.h"

Spline::Spline()
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);
	collider = ChickenWings::game.world.CreateBody(&groundBodyDef);
}

float Spline::sampleHight(float x)
{
	int minIndex = 0;
	int maxIndex = splinePoints.size() - 1;

	for (size_t i = 0; i < 500; i++)
	{
		int index = (maxIndex + minIndex) / 2;
		glm::vec2 leftPoint = splinePoints.at(index);
		glm::vec2 rightPoint = splinePoints.at(index + 1);

		if (x > leftPoint.x && x < rightPoint.x) {
			return sampleHight(x, index);
		}
		else if (x < leftPoint.x) {
			maxIndex = index - 1;
		}
		else if (x > rightPoint.x) {
			minIndex = index + 1;
		}
	}
	return 0;
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

	// we dont have colliders on the first and last segment
	if (splinePoints.size() < 6)
		return;

	std::vector<b2Vec2> splineColliderPoints;
	for (int i = collisionSamplesPerSegment; i >= 0; i--)
	{
		float t = (float)i / collisionSamplesPerSegment;

		glm::vec2 point = getPoint(t, splinePoints.size() - 4);
		splineColliderPoints.push_back(b2Vec2(point.x, point.y));
	}
	float tStepSize = 1.0f / collisionSamplesPerSegment;
	glm::vec2 firstGhostVertex = getPoint(1 - tStepSize, splinePoints.size() - 5);
	b2Vec2 ghostVertex1 = b2Vec2(firstGhostVertex.x, firstGhostVertex.y);
	glm::vec2 secondGhostVertex = getPoint(tStepSize, splinePoints.size() - 3);
	b2Vec2 ghostVertex2 = b2Vec2(secondGhostVertex.x, secondGhostVertex.y);

	b2ChainShape chainShape;
	chainShape.CreateChain(&splineColliderPoints[0], splineColliderPoints.size(), ghostVertex1, ghostVertex2);
	fixtures.push_back(collider->CreateFixture(&chainShape, 0.0f));
	
	// TODO: remove fixtures when removing spline segments
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

glm::vec2 Spline::getTangent(float t, int n)
{
	float x = 0;
	float y = 0;
	glm::vec2 p1 = splinePoints[n];
	glm::vec2 p2 = splinePoints[n + 1];
	glm::vec2 v1 = splinePoints[n + 1] - splinePoints[n - 1];
	glm::vec2 v2 = splinePoints[n + 2] - splinePoints[n];

	float t2 = t * t;
	float t3 = t2 * t; 

	float h00 = 6 * t2 - 6 * t;
	float h10 = 3 * t2 - 4 * t + 1;
	float h01 = -6 * t2 + 6 * t;
	float h11 = 3 * t2 - 2 * t;

	x = h00 * p1.x + h10 * v1.x + h01 * p2.x + h11 * v2.x;
	y = h00 * p1.y + h10 * v1.y + h01 * p2.y + h11 * v2.y;

	glm::vec2 tangent = glm::vec2(x, y);

	tangent = glm::normalize(tangent);

	return tangent;
}

glm::vec2 Spline::getNormal(float t, int n)
{
	glm::vec2 normal = getTangent(t, n);
	float temp = normal.x;
	normal.x = normal.y;
	normal.y = -temp;
	return normal;
}
