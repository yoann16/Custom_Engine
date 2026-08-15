#include <iostream>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef ENGINE_USE_VULKAN
#include <vulkan/vulkan.h>
#endif

#ifdef ENGINE_USE_DX12
#include <dxgi1_6.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif

// -----------------------------------------------------------------------
// Test Vulkan : cree une instance minimale et liste les GPU visibles
// -----------------------------------------------------------------------
#ifdef ENGINE_USE_VULKAN
static void testVulkan()
{
    std::cout << "\n[Vulkan] Initialisation...\n";

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Custom_Engine Build Test";
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[Vulkan] Echec de creation de l'instance (code " << result << ")\n";
        return;
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::cout << "[Vulkan] " << deviceCount << " GPU(s) detecte(s) :\n";
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(device, &props);
        std::cout << "  - " << props.deviceName << "\n";
    }

    vkDestroyInstance(instance, nullptr);
}
#endif

// -----------------------------------------------------------------------
// Test DX12 : liste les adaptateurs GPU via DXGI
// -----------------------------------------------------------------------
#ifdef ENGINE_USE_DX12
static void testDX12()
{
    std::cout << "\n[DX12] Initialisation...\n";

    ComPtr<IDXGIFactory6> factory;
    HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        std::cerr << "[DX12] Echec de creation de la DXGI Factory\n";
        return;
    }

    std::cout << "[DX12] GPU(s) detecte(s) :\n";
    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0;
         factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
         ++i)
    {
        DXGI_ADAPTER_DESC1 desc{};
        adapter->GetDesc1(&desc);
        std::wcout << L"  - " << desc.Description << L"\n";
    }
}
#endif

int main()
{
    std::cout << "=== Custom_Engine : test de build ===\n";

    if (!glfwInit())
    {
        std::cerr << "Echec d'initialisation de GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // pas d'API liee : on ne teste que la creation de fenetre
    GLFWwindow* window = glfwCreateWindow(800, 600, "Custom_Engine - Build Test", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Echec de creation de la fenetre\n";
        glfwTerminate();
        return -1;
    }
    std::cout << "[GLFW] Fenetre creee avec succes.\n";

#ifdef ENGINE_USE_VULKAN
    testVulkan();
#endif

#ifdef ENGINE_USE_DX12
    testDX12();
#endif

    std::cout << "\nAppuie sur ECHAP pour fermer la fenetre.\n";
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
