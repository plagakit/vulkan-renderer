#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Application
{
public:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 600;

	void Run();

private:
	GLFWwindow* m_window;
	VkInstance m_instance;

	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void Cleanup();

};
