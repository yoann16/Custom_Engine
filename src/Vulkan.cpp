#include "Vulkan.h"



const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


void Vulkan::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void Vulkan::createInstance()
{
	constexpr vk::ApplicationInfo appInfo
	{
		.pApplicationName = "Hello Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Custom Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
	};

	//Get the required instance extensions from GLFW
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Check if the required GLFW extensions are supported by the Vulkan implementation.
	auto extensionProperties = context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
	{
		if (std::ranges::none_of(extensionProperties,
								 [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
								 { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
		{
			throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
		}
	}

	vk::InstanceCreateInfo createInfo
	{
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions
	};

	instance = vk::raii::Instance(context, createInfo);
}

void Vulkan::initVulkan()
{
	createInstance();
}

void Vulkan::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void Vulkan::cleanup()
{
	glfwDestroyWindow(window);
	window = nullptr;
	glfwTerminate();
}

void Vulkan::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}
