#pragma once
#include "Spline.h"
#include <memory>
#include <Shader.h>

class SplineRenderer
{
public:
	SplineRenderer(std::weak_ptr<Spline> spline);
	~SplineRenderer();

	glm::vec2 renderOffset;
	int sampleDensity = 10;

	float ybaseLine = 0;

	void Render();
	void Rebuild(bool force = false);

	void SetSpline(std::weak_ptr<Spline> spline);
private:
	glm::vec2 lastStart_;
	glm::vec2 lastEnd_;
	std::weak_ptr<Spline> spline_;

	std::unique_ptr<Shader> shader;

	unsigned int VBO = -1;
	unsigned int VAO = -1;
	unsigned int EBO = -1;


	unsigned int indexCount;
};