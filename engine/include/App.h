#pragma once
#include <string>

struct GLFWwindow;

class App
{
public:
	App(std::string title = "MGJ Game", int width = 800, int height = 600);
	~App();

	void run();

	virtual void StartUp() { };

	virtual void Update() { };

	virtual void Terminate() {}

	GLFWwindow* window;

	const std::string& GetTitle() const;
	void SetTitel(std::string title);

private:
	std::string title_;
};