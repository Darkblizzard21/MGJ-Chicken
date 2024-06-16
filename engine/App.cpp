#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

namespace {
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}

App::App(std::string title, int width, int height): title_(title)
{
	// setup glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setup window
	window = glfwCreateWindow(width, height, title_.c_str(), NULL, NULL);
	glfwSetWindowAspectRatio(window, width, height);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw "fuck";
	}
	glfwMakeContextCurrent(window);


	// load glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw "fuck";
	}


	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	StartUp();
}

App::~App()
{
	Terminate();
	glfwTerminate();
}

void App::run()
{
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// update
		Update();
		// render

		glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// finish
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

const std::string& App::GetTitle() const
{
	return title_;
}

void App::SetTitel(std::string title)
{
	title_ = title;
	glfwSetWindowTitle(window, title.c_str());
}

