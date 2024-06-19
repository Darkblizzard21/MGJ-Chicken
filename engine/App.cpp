#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Shader.h>
#include <box2d/b2_common.h>
#include <Timer.h>

App::App(std::string title, int width, int height) : title_(title), width_(width), height_(height), world(b2Vec2(0, -10.0f))
{
	// setup glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setup window
	window = glfwCreateWindow(width, height, title_.c_str(), NULL, NULL);

	//glfwSetWindowAspectRatio(window, width, height);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw "fuck";
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSwapInterval(0);


	// load glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw "fuck";
	}
	// enable scissors and viewport
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, width_, height_);
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
			constexpr float sixNine = 16.f / 9.f;
			const auto aspect = width * 1.f / height;

			app->width_ = width;
			app->height_ = height;
			app->hPadding_ = 0;
			app->vPadding_ = 0;
			if (width * 1.f / height == sixNine) {
				return;
			}

			if (aspect < sixNine) {
				// crop at top and bottom
				const auto h = (width / 16.f) * 9;
				app->vPadding_ = (height - h) / 2;
			}
			else {
				// crop at sides
				const auto w = (height / 9.f) * 16;
				app->hPadding_ = (width - w) / 2;
			}

			app->ResizeBuffers(width, height);
		});

	// setup buffers
	ResizeBuffers(width, height);

	// set up composit pass
	compositPass_.Initalize();

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
	CleanBuffers();

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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glScissor(0, 0, width_, height_);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 1. geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glScissor(hPadding_, vPadding_, width_ - 2 * hPadding_, height_ - 2 * vPadding_);
		glViewport(hPadding_, vPadding_, width_ - 2 * hPadding_, height_ - 2 * vPadding_);
		glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Timer renderTO("Loop::Render::RenderObjects");
		RenderObjects();
		renderTO.finish();

		Timer renderTQ("Loop::Render::RenderQuads");
		quadManager.RenderQuads();
		renderTQ.finish();

		// 2. lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		compositPass_.Execute(gAlbedo, gNormal, gPosition);

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
			std::cout << "\033[33m" << "Error: FrameRate Dipped below 60!" << "\033[0m" << " Currrent Frame rate: " << std::to_string(1.f / deltaTime_) << std::endl;
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

	Terminate();
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

void App::ResizeBuffers(const int& width, const int& height)
{
	CleanBuffers();

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	// bind
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}

void App::CleanBuffers()
{
	if (gBuffer != -1) {
		glDeleteFramebuffers(1, &gBuffer);
		gBuffer = -1;
	}
	if (gPosition != -1) {
		glDeleteTextures(1, &gPosition);
		gPosition = -1;
	}
	if (gNormal != -1) {
		glDeleteTextures(1, &gNormal);
		gNormal = -1;
	}
	if (gAlbedo != -1) {
		glDeleteTextures(1, &gAlbedo);
		gAlbedo = -1;
	}
}

