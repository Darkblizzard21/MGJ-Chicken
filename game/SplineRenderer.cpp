#include "SplineRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <UberShader.h>

SplineRenderer::SplineRenderer(std::weak_ptr<Spline> spline)
{
	SetSpline(spline);
}

SplineRenderer::~SplineRenderer()
{
	buffers.cleanUp();
}

void SplineRenderer::Render()
{
	if (spline_.expired()) return;
	Rebuild(lastStart_ == lastEnd_ || !buffers.valid());

	UberData data = {};
	data.position = renderOffset;
	data.scale = { 1,1 };
	data.layer = layer;
	data.rotation = 0;
	data.uvOffset = uvOffset;
	data.uvScale = { 0.5,0.5 };
	UberShader::DrawElements(data, buffers);
}

void SplineRenderer::Rebuild(bool force)
{
	if (spline_.expired()) {
		std::cout << "SplineRenderer::Rebuild: Spline was expired" << std::endl;
		return;
	};
	auto spline = spline_.lock();
	const auto& splinePoints = spline->splinePoints;
	if (!force &&
		lastStart_ == splinePoints[0] &&
		lastEnd_ == splinePoints[splinePoints.size() - 1]) {
		return;
	}
	if (splinePoints.size() < 4) {
		std::cout << "SplineRenderer::Rebuild: Spline was to short" << std::endl;
		return;
	}

	buffers.cleanUp();

	std::vector<UberVertex> vertices = {};
	std::vector<glm::uint32_t> indices = {};

	auto firstPoint = spline->getPoint(0.f, 1);
	vertices.push_back(UberVertex(firstPoint.x, ybaseLine));
	vertices.push_back(UberVertex(firstPoint.x, std::max(firstPoint.y, ybaseLine)));
	for (size_t s = 1; s < splinePoints.size() - 2; s++)
	{
		for (size_t i = 0; i < sampleDensity; i++)
		{
			float t = (i + 1.f) / (sampleDensity);
			std::cout << "I: " << i << " T: " << t << std::endl;
			auto point = spline->getPoint(t, (int)s);

			vertices.push_back(UberVertex(point.x, ybaseLine));
			vertices.push_back(UberVertex(point.x, std::max(point.y, ybaseLine)));

			 const auto size = (glm::uint32_t) vertices.size();

			indices.push_back(size - 4);
			indices.push_back(size - 3);
			indices.push_back(size - 2);

			indices.push_back(size - 3);
			indices.push_back(size - 1);
			indices.push_back(size - 2);
		}
	}

	buffers = UberShader::UploadMesh(vertices, indices);

	assert(indices.size() % 3 == 0);
	triangleCount = static_cast<unsigned int>(indices.size() / 3);

	lastStart_ = splinePoints[0];
	lastEnd_ = splinePoints[splinePoints.size() - 1];
}

void SplineRenderer::SetSpline(std::weak_ptr<Spline> spline)
{
	spline_ = spline;
	lastStart_ = { 0,0 };
	lastEnd_ = lastStart_;
}
