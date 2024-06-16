#pragma once
#include <string>
#include <Quad.h>
#include <chrono>

struct GLFWwindow;

class App
{
public:
	App(std::string title = "MGJ Game", int width = 1600, int height = 900);
	~App();

	void run();

	virtual void StartUp() { };

	virtual void Update() { };

	virtual void Terminate() {}


	float deltaTime();
	float gameTime();

	const std::string& GetTitle() const;
	void SetTitel(std::string title);

	float targetFrameRate = 60;

	GLFWwindow* window;

	std::unique_ptr<QuadManager> quadManager;
private:
	std::string title_;

	float gameTime_;
	float deltaTime_;

	std::chrono::steady_clock::time_point gameStart_;
	std::chrono::steady_clock::time_point lastFrame_;
};