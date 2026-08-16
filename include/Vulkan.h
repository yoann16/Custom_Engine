#ifndef VULKAN_H
#define VULKAN_H
#pragma once

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Vulkan
{
private:
	GLFWwindow* window = nullptr;

private:
	void initWindow();

	void initVulkan();

	void mainLoop();

	void cleanup();

public:
	void run();
};

#endif

