#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <vector>
#include <iostream>

#include "queue_family.h"

class Application
{
public:
	static constexpr uint32_t s_WIDTH = 800;
	static constexpr uint32_t s_HEIGHT = 600;

	void Run();

private:
	GLFWwindow* m_window;
	VkInstance m_instance;

	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	VkDevice m_device; // logical device
	VkQueue m_graphicsQueue;

	void InitWindow();
	void InitVulkan();

#pragma region Instance Creation
	void CreateVulkanInstance();

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
#pragma endregion

#pragma region Physical Device Creation
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
#pragma endregion

#pragma region Logical Device Creation
	void CreateLogicalDevice();
#pragma endregion

	void MainLoop();

	void Cleanup();


	static constexpr size_t s_validationLayerCount = 1;
	static constexpr std::array<const char*, s_validationLayerCount> s_validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
#ifdef NDEBUG
	static constexpr bool s_enableValidationLayers = false;
#else
	static constexpr bool s_enableValidationLayers = true;
#endif

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};
