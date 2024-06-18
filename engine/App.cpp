#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Shader.h>
#include <box2d/b2_common.h>
#include <Timer.h>

namespace {
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}

App::App(std::string title, int width, int height) : title_(title), world(b2Vec2(0, -10.0f))
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
	glfwSwapInterval(0);


	// load glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw "fuck";
	}


	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// enalbe depth	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	UberShader::Initialize();
	// setup game systems
	gameTime_ = 0;
	deltaTime_ = 1 / targetFrameRate;

	quadManager.Initialize();
}

App::~App()
{
	Terminate();
	glfwTerminate();
}

void App::run()
{
	StartUp();

	gameStart_ = std::chrono::steady_clock::now();
	lastFrame_ = gameStart_;

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// update physics
		Timer physicsT("Loop::Physiks");
		world.Step(1 / targetFrameRate, velocityIterations, positionIterations);
		physicsT.finish();
		// update


		Timer updateT("Loop::Update");
		Update();
		updateT.finish();

		
		Timer renderT("Loop::Render");

		// render
		glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Timer renderTO("Loop::Render::RenderObjects");
		RenderObjects();
		renderTO.finish();

		Timer renderTQ("Loop::Render::RenderQuads");
		quadManager.RenderQuads();
		renderTQ.finish();

		// finish
		Timer renderTS("Loop::Render::glfwSwapBuffers");
		glfwSwapBuffers(window);
		renderTS.finish();
		renderT.finish();
		glfwPollEvents();

		const float deltaTarget = 1 / targetFrameRate;
		std::chrono::steady_clock::time_point currentTime;
		size_t waitCount = 0;
		do
		{
			waitCount++;
			currentTime = std::chrono::steady_clock::now();
			deltaTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrame_).count() * 0.001f;

		} while (deltaTime_ < deltaTarget);
		if (waitCount == 1) {
			std::cout << "\033[33m" << "Error: FrameRate Dipped below 60!" << "\033[0m" << " Currrent Frame rate: " << std::to_string(1.f/ deltaTime_) << std::endl;
		}
		else {
			physicsT.print = false;
			updateT.print = false;
			renderT.print = false;
			renderTO.print = false; 
			renderTQ.print = false;
			renderTS.print = false; 
		}
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

