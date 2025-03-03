#include "application.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main()
{
	Application app;

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;

	//glfwInit();

	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Renderer", nullptr, nullptr);

	//uint32_t extensionCount = 0;
	//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	//std::cout << extensionCount << " extensions supported\n";

	//glm::mat4 matrix;
	//glm::vec4 vec;
	//auto test = matrix * vec;

	//while (!glfwWindowShouldClose(window))
	//	glfwPollEvents();

	//glfwDestroyWindow(window);

	//glfwTerminate();
}