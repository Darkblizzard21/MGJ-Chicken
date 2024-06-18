#pragma once
#include "Spline.h"
#include <memory>
#include "UberShader.h"

enum class SplineMode
{
	FillBelow,
	Line
};

class SplineRenderer
{
public:
	SplineRenderer(std::shared_ptr<Spline> spline, SplineMode mode = SplineMode::Line);
	~SplineRenderer();

	int sampleDensity = 15;

	const SplineMode mode;
	// FillBelow
	float ybaseLine = 0;
	// Line
	float upperWidth = 0.2f;
	float lowerWidth = 0.2f;

	uint8_t layer = 65;
	glm::vec2 renderOffset = { 0,0 };
	glm::vec2 uvOffset = { 0,0 };

	void Render();
	void Rebuild(bool force = false);

	void SetSpline(std::shared_ptr<Spline> spline);
private:
	glm::vec2 lastStart_;
	glm::vec2 lastEnd_;
	std::shared_ptr<Spline> spline_;

	struct SplineSegment
	{
		glm::vec2 startPoint;
		MeshBuffers buffers;
	};

	SplineSegment CreateSplineSegmentFor(const int& i);
	
	glm::vec4 GetVertices(float x, int n);

	std::vector<SplineSegment> splineSegments;
};