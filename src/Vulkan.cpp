#include "Vulkan.h"

#include <iostream>
#include <map>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp



const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<char const*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif



void Vulkan::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

std::vector<char const*> Vulkan::getRequiredInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
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

	//Get the required layers
	std::vector<char const*> requiredLayers;
	if (enableValidationLayers)
	{
		requiredLayers.assign(validationLayers.begin(), validationLayers.end());
	}

	//Check if the required layers are supported by the Vulkan implementation
	auto layerProperties = context.enumerateInstanceLayerProperties();
	auto unsupportedLayerIt =
			std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer)
			{
					return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
					{
						return strcmp(layerProperty.layerName, requiredLayer) == 0;
					});
			});

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}

	//Get the required extensions
	auto requiredExtensions = getRequiredInstanceExtensions();

	//Check if the required extensions are supported by the Vulkan implementation.
	auto extensionProperties = context.enumerateInstanceExtensionProperties();
	auto unsupportedPropertyIt =
			std::ranges::find_if(requiredExtensions, [&extensionProperties](auto const& requiredExtension)
			{
					return std::ranges::none_of(extensionProperties, [requiredExtension](auto const& extensionProperty)
					{
						return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
					});
			});

	if (unsupportedPropertyIt != requiredExtensions.end())
	{
		throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
	}


	vk::InstanceCreateInfo createInfo
	{
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	instance = vk::raii::Instance(context, createInfo);
}

vk::Bool32 Vulkan::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
								 vk::DebugUtilsMessageTypeFlagsEXT type,
								 const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
								 void* pUserData)
{
	std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

	return vk::False;
}

void Vulkan::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
														vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral       |
													   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
													   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT
	{
		.messageSeverity = severityFlags,
		.messageType = messageTypeFlags,
		.pfnUserCallback = &debugCallback
	};

	debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void Vulkan::createSurface()
{
	VkSurfaceKHR _surface;

	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
		throw std::runtime_error("failed to create window surface !");

	surface = vk::raii::SurfaceKHR(instance, _surface);
}

bool Vulkan::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	//Check if physicalDevice support geometry shaders
	auto deviceFeatures = physicalDevice.getFeatures();
	bool supportGeometryShaders = deviceFeatures.geometryShader;

	//Check if the physicalDevice support the Vulkan 1.3 API version
	bool supportVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	//Check if any of the queue families support graphics operations
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics =
			std::ranges::any_of(queueFamilies, [](auto const& qfp)
			{
				return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
			});

	//Check if all required physicalDevice extensions are available
	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions =
			std::ranges::all_of(requiredDeviceExtension, [&availableDeviceExtensions](auto const& requiredDeviceExtension)
			{
				return std::ranges::any_of(availableDeviceExtensions, [requiredDeviceExtension](auto const& availableDeviceExtension)
					{
						return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
					});
			});

	//Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
																		vk::PhysicalDeviceVulkan11Features,
																		vk::PhysicalDeviceVulkan13Features,
																		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
									features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
									features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	//Return true if the physicalDevice meets all the criteria
	return supportGeometryShaders && supportVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void Vulkan::pickPhysicalDevice()
{
	auto physicalDevices = instance.enumeratePhysicalDevices();

	if (physicalDevices.empty())
		throw std::runtime_error("failed to find GPUs with Vulkan support !");
	
	//Use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, vk::raii::PhysicalDevice> candidates;

	for (const auto& pd : physicalDevices)
	{
		auto deviceProperties = pd.getProperties();
		
		uint32_t score = 0;

		if (!isDeviceSuitable(pd))
			continue;

		//Discrete GPUs have a significant perfomance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			score += 1000;
		}

		//Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		candidates.insert(std::make_pair(score, pd));
	}

	//Check if the best candidate is suitable at all
	if (!candidates.empty() /*&& candidates.rbegin()->first > 0*/) 
		/**This condition is redundant with `candidates.empty`, 
		since a GPU that supports Vulkan cannot have a score of <= 0**/
		physicalDevice = candidates.rbegin()->second;
	else
		throw std::runtime_error("failed to find a suitable GPU !");
}

void Vulkan::createLogicalDevice()
{
	//find the index of the first queue family that supports graphics
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	//get the first index into queueFamilyProperties which supports both graphics and presents
	uint32_t queueIndex = ~0;
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
	{
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
		{
			//found a queue family that supports both graphics and present
			queueIndex = qfpIndex;
			break;
		}
	}

	if (queueIndex == ~0)
	{
		throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
	}

	// query for Vulkan 1.3 features
	vk::StructureChain<vk::PhysicalDeviceFeatures2,
					   vk::PhysicalDeviceVulkan11Features,
					   vk::PhysicalDeviceVulkan13Features,
					   vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
		featureChain =
		{
		{},								//vk::PhysicalDeviceFeatures2
		{.shaderDrawParameters = true},   //vk::PhysicalDeviceVulkan11Features
		{.dynamicRendering = true},		//vk::PhysicalDeviceVulkan13Features
		{.extendedDynamicState = true}	//vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
		};

	// create a device
	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo
	{
		.queueFamilyIndex = queueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};
	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
		.ppEnabledExtensionNames = requiredDeviceExtension.data()
	};

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	graphicsQueue = vk::raii::Queue(device, queueIndex, 0);
}

uint32_t Vulkan::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}

	return minImageCount;
}

vk::SurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
	const auto formatIt =
			std::ranges::find_if(availableFormats, [](const auto& format)
			{
				return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
			});

	return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR Vulkan::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
	assert
		(std::ranges::any_of(availablePresentModes, [](auto presentMode)
		{
			return presentMode == vk::PresentModeKHR::eFifo;
		}));

	return 
		std::ranges::any_of(availablePresentModes, [](const vk::PresentModeKHR value)
		{
			return vk::PresentModeKHR::eMailbox == value;
		}) ?
		vk::PresentModeKHR::eMailbox :
			vk::PresentModeKHR::eFifo;
}

vk::Extent2D Vulkan::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return
	{
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

void Vulkan::createSwapChain()
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	swapChainExtent = chooseSwapExtent(surfaceCapabilities);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
	swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo
	{
		.surface = *surface,
		.minImageCount = minImageCount,
		.imageFormat = swapChainSurfaceFormat.format,
		.imageColorSpace = swapChainSurfaceFormat.colorSpace,
		.imageExtent = swapChainExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = true
	};

	swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
	swapChainImages = swapChain.getImages();
}


void Vulkan::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
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
