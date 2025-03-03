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

	m_window = glfwCreateWindow(s_WIDTH, s_HEIGHT, "Vulkan Renderer", nullptr, nullptr);
}

void Application::InitVulkan()
{
	std::cout << "Initializing Vulkan..." << std::endl;

	CreateVulkanInstance();
	SetupDebugMessenger();
	PickPhysicalDevice();
	CreateLogicalDevice();
}

#pragma region Instance Creation

void Application::CreateVulkanInstance()
{
	if (s_enableValidationLayers && !CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested but not available!");

	// Tell Vulkan info about our app
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Tell Vulkan which global extensions & validation layers we wanna use
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	
	// Enable debugging for instance creation
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (s_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayerCount);
		createInfo.ppEnabledLayerNames = s_validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else 
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance!");
}

bool Application::CheckValidationLayerSupport()
{
	// Load all available validation layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Check if all of our layers are in the available list
	for (const auto& layerName : s_validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}

	return true;
}

std::vector<const char*> Application::GetRequiredExtensions()
{
	uint32_t glfwExtCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

	std::vector<const char*> exts(glfwExtensions, glfwExtensions + glfwExtCount);

	if (s_enableValidationLayers)
		exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return exts;
}

void Application::SetupDebugMessenger()
{
	if (!s_enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo(createInfo);

	VkResult result = CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger!");
}

void Application::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

#pragma endregion

#pragma region Physical Device Creation
void Application::PickPhysicalDevice()
{
	// Get all GPUs
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	// Pick first that meets reqs
	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to GPU that meets requirements!");
}

bool Application::IsDeviceSuitable(VkPhysicalDevice device)
{
	// Ensure device has features and properties we want to use
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// Ensure device can process commands we want it to
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return //deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		indices.IsComplete() &&
		deviceFeatures.geometryShader;
}

QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	
	// Assign index to queue families that could be found
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.m_graphicsFamily = i;

		// Choose first good family we find
		if (indices.IsComplete())
			break;

		i++;
	}
	
	return indices;
}
#pragma endregion

void Application::CreateLogicalDevice()
{
	// Queue family info
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.m_graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// Physical device info
	VkPhysicalDeviceFeatures deviceFeatures{};

	// Create the logical device
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	if (s_enableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
		createInfo.ppEnabledLayerNames = s_validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device!");

	// Queues are implicitly created by logical device creation, but we need handle
	vkGetDeviceQueue(m_device, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue);
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

	vkDestroyDevice(m_device, nullptr);

	if (s_enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}

	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

