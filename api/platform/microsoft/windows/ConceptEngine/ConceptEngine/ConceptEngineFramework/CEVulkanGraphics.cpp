#include "CEVulkanGraphics.h"

#include "CEOSTools.h"
#include "CEVulkanManager.h"
#include "CEWindow.h"

CEVulkanGraphics::CEVulkanGraphics(HWND hWnd): CEGraphics(hWnd, CEOSTools::CEGraphicsApiTypes::vulkan) {
    VkResult result;
    VkInstance instance;
    VkSurfaceKHR surface;
    //TODO: Load vulkan libraries dynamically
    VkApplicationInfo applicationInfo = { };
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Concept Engine";
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceInfo = { };
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = 3;

    // result = vkCreateInstance(&instanceInfo, nullptr, &instance);

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = hWnd;

    // result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
}

void CEVulkanGraphics::EndFrame() {
}

void CEVulkanGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEVulkanGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
                                         CEDefaultFigureTypes figureTypes) {
}

CEGraphicsManager CEVulkanGraphics::GetGraphicsManager() {
	return static_cast<CEGraphicsManager>(CEVulkanManager());
}

void CEVulkanGraphics::PrintGraphicsVersion() {
}
