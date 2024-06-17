#include "SplineRenderer.h"
#include <glad/glad.h>
#include <iostream>

SplineRenderer::SplineRenderer(std::weak_ptr<Spline> spline)
{
	SetSpline(spline);

	const std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0);\n"
		"}\0";
	const std::string fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1,1,1,1);\n"
		"}\n\0";

	shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
}

SplineRenderer::~SplineRenderer()
{
	if (VBO != -1) {
		glDeleteBuffers(1, &VBO);
	}
	if (VAO != -1) {
		glDeleteBuffers(1, &VAO);
	}
	if (EBO != -1) {
		glDeleteBuffers(1, &EBO);
	}
}

void SplineRenderer::Render()
{
	if (spline_.expired()) return;
	Rebuild(lastStart_ == lastEnd_);

	shader->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

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

	// cleanup
	if (VBO != -1) {
		glDeleteBuffers(1, &VBO);
	}
	if (VAO != -1) {
		glDeleteBuffers(1, &VAO);
	}
	if (EBO != -1) {
		glDeleteBuffers(1, &EBO);
	}

	std::vector<glm::vec2> vertices = {};
	std::vector<glm::uint32_t> indices = {};

	auto firstPoint = spline->getPoint(0.f, 1);
	vertices.push_back(glm::vec2(firstPoint.x, ybaseLine));
	vertices.push_back(glm::vec2(firstPoint.x, std::max(firstPoint.y, ybaseLine)));
	for (size_t s = 1; s < splinePoints.size() - 2; s++)
	{
		for (size_t i = 0; i < sampleDensity; i++)
		{
			float t = (i + 1.f) / (sampleDensity);
			std::cout << "I: " << i << " T: " << t << std::endl;
			auto point = spline->getPoint(t, s);

			vertices.push_back(glm::vec2(point.x, ybaseLine));
			vertices.push_back(glm::vec2(point.x, std::max(point.y, ybaseLine)));

			glm::uint32_t size = vertices.size();

			indices.push_back(size - 4);
			indices.push_back(size - 3);
			indices.push_back(size - 2);

			indices.push_back(size - 3);
			indices.push_back(size - 1);
			indices.push_back(size - 2);
		}
	}

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uint32_t), indices.data(), GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers

	// Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	assert(indices.size() % 3 == 0);
	indexCount = indices.size();

	lastStart_ = splinePoints[0];
	lastEnd_ = splinePoints[splinePoints.size() - 1];
}

void SplineRenderer::SetSpline(std::weak_ptr<Spline> spline)
{
	spline_ = spline;
	lastStart_ = { 0,0 };
	lastEnd_ = lastStart_;
}
