#pragma once
#include "Spline.h"
#include <memory>
#include "UberShader.h"

class SplineRenderer
{
public:
	SplineRenderer(std::shared_ptr<Spline> spline);
	~SplineRenderer();

	int sampleDensity = 15;

	float ybaseLine = 0;
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

	struct SplinePiece
	{
		glm::vec2 startPoint;
		MeshBuffers buffers;
	};

	SplinePiece CreateSplinePieceFor(const int& i);

	std::vector<SplinePiece> splinePieces;
};