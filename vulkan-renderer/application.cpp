#include "application.h"

#include <iostream>

void Application::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

void Application::InitWindow()
{
	std::cout << "Initializing window..." << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // tell GLFW we not using OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // disable resizing

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Renderer", nullptr, nullptr);
}

void Application::InitVulkan()
{
	std::cout << "Initializing Vulkan..." << std::endl;

	// Create instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
}

void Application::MainLoop()
{
	std::cout << "Starting up main loop..." << std::endl;

	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void Application::Cleanup()
{
	std::cout << "Shutting down & cleaning up..." << std::endl;

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

