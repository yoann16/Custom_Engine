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

void Vulkan::initVulkan()
{

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
	glfwTerminate();
}

void Vulkan::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}
