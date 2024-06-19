#include "SplineRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <UberShader.h>
#include <Timer.h>

SplineRenderer::SplineRenderer(std::shared_ptr<Spline> spline, SplineMode mode) : mode(mode)
{
	SetSpline(spline);
}

SplineRenderer::~SplineRenderer()
{
	for (size_t i = 0; i < splineSegments.size(); i++)
	{
		splineSegments[i].buffers.cleanUp();
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
	data.uvScale = uvScale;
	data.colorTexture = texture;
	for (const auto& segment : splineSegments)
	{
		if (UberShader::InFrustum(segment.buffers, renderOffset)) {
			UberShader::DrawElements(data, segment.buffers);
		}
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

	while (!splineSegments.empty() && splineSegments[0].startPoint != splinePoints[1])
	{
		splineSegments[0].buffers.cleanUp();
		splineSegments.erase(splineSegments.begin());
	}
	if (splineSegments.empty()) {
		for (size_t i = 1; i < splinePoints.size() - 2; i++)
		{
			splineSegments.push_back(CreateSplineSegmentFor(i));
		}
	}
	else {
		size_t i;
		const auto& lastStart = splineSegments[splineSegments.size() - 1].startPoint;
		for (i = 0; i < splinePoints.size(); i++)
		{
			if (splinePoints[i] == lastStart) {
				break;
			}
		}
		for (i = i + 1; i < splinePoints.size() - 2; i++)
		{
			splineSegments.push_back(CreateSplineSegmentFor(i));
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

SplineRenderer::SplineSegment SplineRenderer::CreateSplineSegmentFor(const int& n)
{
	SplineSegment result;

	std::vector<UberVertex> vertices = {};
	std::vector<glm::uint32_t> indices = {};

	const auto startPoint = spline_->getPoint(0.f, n);
	const auto endPoint = spline_->getPoint(1.f, n);

	glm::vec4 startVerts = GetVertices(0.f, n, true);
	vertices.push_back(UberVertex(startVerts.x, startVerts.y));
	vertices.push_back(UberVertex(startVerts.z, startVerts.w));


	for (size_t i = 0; i < (sampleDensity - 1); i++)
	{
		float t = (i + 1.f) / (sampleDensity);
		float x = glm::mix(startPoint.x, endPoint.x, t);
		glm::vec4 verts = GetVertices(x, n);

		vertices.push_back(UberVertex(verts.x, verts.y));
		vertices.push_back(UberVertex(verts.z, verts.w));

		const auto size = (glm::uint32_t)vertices.size();

		indices.push_back(size - 4);
		indices.push_back(size - 3);
		indices.push_back(size - 2);

		indices.push_back(size - 3);
		indices.push_back(size - 1);
		indices.push_back(size - 2);
	}


	glm::vec4 endVerts = GetVertices(1.f, n, true);
	vertices.push_back(UberVertex(endVerts.x, endVerts.y));
	vertices.push_back(UberVertex(endVerts.z, endVerts.w));

	const auto size = (glm::uint32_t)vertices.size();

	indices.push_back(size - 4);
	indices.push_back(size - 3);
	indices.push_back(size - 2);

	indices.push_back(size - 3);
	indices.push_back(size - 1);
	indices.push_back(size - 2);

	assert(indices.size() % 3 == 0);
	result.startPoint = startPoint;
	result.buffers = UberShader::UploadMesh(vertices, indices);
	return result;
}

glm::vec4 SplineRenderer::GetVertices(float xt, int n, bool useT)
{
	glm::vec4 result = {};
	switch (mode)
	{
	case SplineMode::FillBelow: {
		float x = xt;
		float y = 0;
		if (useT) {
			const auto p = spline_->getPoint(xt, n);
			x = p.x;
			y = p.y;
		}
		else {
			y = spline_->sampleHight(xt, n);
		}

		result.x = x;
		result.y = ybaseLine;
		result.z = x;
		result.w = std::max(y, ybaseLine);
		break;
	}
	case SplineMode::Line: {
		float t = useT ? xt : spline_->sampleT(xt, n);
		const glm::vec2 p = spline_->getPoint(t, n);
		const glm::vec2 normal = spline_->getNormal(t, n);

		const glm::vec2 l = p - normal * lowerWidth;
		const glm::vec2 u = p + normal * upperWidth;
		result.x = l.x;
		result.y = l.y;
		result.z = u.x;
		result.w = u.y;
		break;
	}
	default:
		std::cout << "Unkown Spline Type!" << std::endl;
		break;
	}
	return result;
}

