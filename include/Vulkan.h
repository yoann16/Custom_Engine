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
	
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

private:
	void createInstance();
	std::vector<const char*> getRequiredInstanceExtensions();
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
														  vk::DebugUtilsMessageTypeFlagsEXT             type,
														  const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
														  void*											pUserData);
	void setupDebugMessenger();

private:
	void initWindow();

	void initVulkan();

	void mainLoop();

	void cleanup();

public:
	void run();
};

#endif

