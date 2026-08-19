#ifndef VULKAN_H
#define VULKAN_H
#pragma once
#include <vector>


#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Vulkan
{
private:
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
private:
	GLFWwindow* window = nullptr;
	
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;
	std::vector<vk::raii::ImageView> swapChainImageViews;

private:
	void createInstance();
	std::vector<const char*> getRequiredInstanceExtensions();
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
														  vk::DebugUtilsMessageTypeFlagsEXT             type,
														  const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
														  void*											pUserData);
	void setupDebugMessenger();
	void createSurface();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void pickPhysicalDevice();
	void createLogicalDevice();
	static uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	static vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);
	void createSwapChain();
	void createImageViews();

private:
	void initWindow();

	void initVulkan();

	void mainLoop();

	void cleanup();

public:
	void run();
};

#endif

