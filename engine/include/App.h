#pragma once
#include <string>
#include <Quad.h>

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

	GLFWwindow* window;

	const std::string& GetTitle() const;
	void SetTitel(std::string title);

	std::unique_ptr<QuadManager> quadManager;
private:
	std::string title_;
};