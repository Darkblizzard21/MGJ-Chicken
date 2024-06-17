#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Shader.h>

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


	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// setup game systems
	quadManager.Initialize();

	StartUp();
}

App::~App()
{
	Terminate();
	glfwTerminate();
}

void App::run()
{
	gameStart_ = std::chrono::steady_clock::now();
	lastFrame_ = gameStart_;
	gameTime_ = 0;
	deltaTime_ = 1 / targetFrameRate;
	while (!glfwWindowShouldClose(window))
	{
		std::cout << "GameTime: " << gameTime_ << " FrameTime: " << deltaTime_ << std::endl;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// update
		Update();
		// render
		glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		quadManager.RenderQuads();

		// finish
		glfwSwapBuffers(window);
		glfwPollEvents();

		const float deltaTarget = 1 / targetFrameRate;
		std::chrono::steady_clock::time_point currentTime;
		do
		{
			currentTime = std::chrono::steady_clock::now();
			deltaTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrame_).count() * 0.001f;

		} while (deltaTime_ < deltaTarget);
		gameTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - gameStart_).count() * 0.001f;
		lastFrame_ = currentTime;
	}
}

float App::deltaTime()
{
	return deltaTime_;
}

float App::gameTime()
{
	return gameTime_;
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

