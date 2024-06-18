#include "SplineRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <UberShader.h>
#include <Timer.h>

SplineRenderer::SplineRenderer(std::shared_ptr<Spline> spline)
{
	SetSpline(spline);
}

SplineRenderer::~SplineRenderer()
{
	for (size_t i = 0; i < splinePieces.size(); i++)
	{
		splinePieces[i].buffers.cleanUp();
	}
}

void SplineRenderer::Render()
{
	if (spline_ == nullptr) return;
	Rebuild(lastStart_ == lastEnd_);

	UberData data = {};
	data.position = renderOffset;
	data.scale = { 1,1 };
	data.layer = layer;
	data.rotation = 0;
	data.uvOffset = uvOffset;
	data.uvScale = { 0.5,0.5 };
	for (const auto& segment: splinePieces)
	{
		UberShader::DrawElements(data, segment.buffers);
	}
}

void SplineRenderer::Rebuild(bool force)
{
	if (spline_ == nullptr || spline_.use_count() == 1) {
		std::cout << "SplineRenderer::Rebuild: Spline was expired" << std::endl;
		spline_ = nullptr;
		return;
	};
	const auto& splinePoints = spline_->splinePoints;
	if (!force &&
		lastStart_ == splinePoints[0] &&
		lastEnd_ == splinePoints[splinePoints.size() - 1]) {
		return;
	}
	if (splinePoints.size() < 4) {
		std::cout << "SplineRenderer::Rebuild: Spline was to short" << std::endl;
		return;
	}
	Timer timer("SplineRenderer::Rebuild");

	while (!splinePieces.empty() && splinePieces[0].startPoint != splinePoints[1])
	{
		splinePieces[0].buffers.cleanUp();
		splinePieces.erase(splinePieces.begin());
	}
	if (splinePieces.empty()) {
		for (size_t i = 1; i < splinePoints.size() - 2; i++)
		{
			splinePieces.push_back(CreateSplinePieceFor(i));
		}
	}
	else {
		size_t i;
		const auto& lastStart = splinePieces[splinePieces.size() - 1].startPoint;
		for (i = 0; i < splinePoints.size(); i++)
		{
			if (splinePoints[i] == lastStart) {
				break;
			}
		}
		for (i = i + 1; i < splinePoints.size() - 2; i++)
		{
			splinePieces.push_back(CreateSplinePieceFor(i));
		}
	}
	

	lastStart_ = splinePoints[0];
	lastEnd_ = splinePoints[splinePoints.size() - 1];
}

void SplineRenderer::SetSpline(std::shared_ptr<Spline> spline)
{
	spline_ = spline;
	lastStart_ = { 0,0 };
	lastEnd_ = lastStart_;
}

SplineRenderer::SplinePiece SplineRenderer::CreateSplinePieceFor(const int& n)
{
	SplinePiece result;

	std::vector<UberVertex> vertices = {};
	std::vector<glm::uint32_t> indices = {};

	result.startPoint = spline_->getPoint(0.f, n);
	vertices.push_back(UberVertex(result.startPoint.x, ybaseLine));
	vertices.push_back(UberVertex(result.startPoint.x, std::max(result.startPoint.y, ybaseLine)));

	for (size_t i = 0; i < sampleDensity; i++)
	{
		float t = (i + 1.f) / (sampleDensity);
		auto point = spline_->getPoint(t, n);

		vertices.push_back(UberVertex(point.x, ybaseLine));
		vertices.push_back(UberVertex(point.x, std::max(point.y, ybaseLine)));

		const auto size = (glm::uint32_t)vertices.size();

		indices.push_back(size - 4);
		indices.push_back(size - 3);
		indices.push_back(size - 2);

		indices.push_back(size - 3);
		indices.push_back(size - 1);
		indices.push_back(size - 2);
	}


	assert(indices.size() % 3 == 0);
	result.buffers = UberShader::UploadMesh(vertices, indices);
	return result;
}
