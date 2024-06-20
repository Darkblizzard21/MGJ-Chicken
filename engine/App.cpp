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
#ifdef DEBUG
	compositPass_.EnableDebug();
#endif

	// enalbe depth	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UberShader::Initialize();
	// setup game systems
	gameTime_ = 0;
	deltaTime_ = 1 / targetFrameRate;

	quadManager.Initialize();
	uiManager.Initialize();
	numberManager.Initialize(&uiManager);

	backgroundQuad = quadManager.CreateQuad();
	backgroundQuad->layer = 0;
	backgroundQuad->scale = glm::vec2(17, 10);
	std::vector<glm::vec3> fallback = { glm::vec3(0.6f, 0.3f, 0.3f) };
	backgroundQuad->colorTexture = std::make_shared<Texture>(fallback, 1, 1);
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


	float f1FlipTime = -1.f;
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && f1FlipTime < 0.0f) {
			f1FlipTime = 0.2f;
			compositPass_.FlipDebug();
			std::cout << "flip" << std::endl;
		}
		else {
			f1FlipTime = std::max(f1FlipTime - deltaTime(), -1.f);
		}

		// update physics
		Timer physicsT("Loop::Physiks");
		world.Step(1 / targetFrameRate, velocityIterations, positionIterations);
		physicsT.finish();
		// update


		Timer updateT("Loop::Update");
		Update();
		updateT.finish();
		numberManager.Update();

		Timer renderT("Loop::Render");
		backgroundQuad->position = UberShader::cameraPosition;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glScissor(0, 0, width_, height_);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 1. geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glScissor(hPadding_, vPadding_, width_ - 2 * hPadding_, height_ - 2 * vPadding_);
		glViewport(hPadding_, vPadding_, width_ - 2 * hPadding_, height_ - 2 * vPadding_);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Timer renderTO("Loop::Render::RenderObjects");
		RenderObjects();
		renderTO.finish();

		Timer renderTQ("Loop::Render::RenderQuads");
		quadManager.RenderQuads();
		renderTQ.finish();

		// 2. lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		compositPass_.Execute(gAlbedo, gNormal, gLayer);

		// 3. UI pass
		if (compositPass_.GetDebug()) {
			const auto hP = width_ / 2 + hPadding_ / 2;
			const auto vP = vPadding_ / 2;
			const auto w = (width_ - 2 * hPadding_) / 2;
			const auto h = (height_ - 2 * vPadding_) / 2;
			glScissor(hP, vP, w, h);
			glViewport(hP, vP, w, h);
		}
		glEnable(GL_BLEND);
		const auto cameraPos = UberShader::cameraPosition;
		UberShader::cameraPosition = glm::vec2(0, 0);
		Timer renderTQUI("Loop::Render::UI");
		uiManager.RenderQuads();
		renderTQUI.finish();
		UberShader::cameraPosition = cameraPos;
		glDisable(GL_BLEND);
#
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
		if (waitCount == 1 && false) {
			std::cout << "\033[33m" << "Error: FrameRate Dipped below 60!" << "\033[0m" << " Currrent Frame rate: " << std::to_string(1.f / deltaTime_) << std::endl;
		}
		else {
			physicsT.print = false;
			updateT.print = false;
			renderT.print = false;
			renderTO.print = false;
			renderTQ.print = false;
			renderTS.print = false;
			renderTQUI.print = false;
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

	// - color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);


	// - position color buffer
	glGenTextures(1, &gLayer);
	glBindTexture(GL_TEXTURE_2D, gLayer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gLayer, 0);

	// bind
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete! resize again!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::CleanBuffers()
{
	if (rboDepth != -1) {
		glDeleteRenderbuffers(1, &rboDepth);
		rboDepth = -1;
	}

	if (gLayer != -1) {
		glDeleteTextures(1, &gLayer);
		gLayer = -1;
	}
	if (gNormal != -1) {
		glDeleteTextures(1, &gNormal);
		gNormal = -1;
	}
	if (gAlbedo != -1) {
		glDeleteTextures(1, &gAlbedo);
		gAlbedo = -1;
	}

	if (gBuffer != -1) {
		glDeleteFramebuffers(1, &gBuffer);
		gBuffer = -1;
	}
}

