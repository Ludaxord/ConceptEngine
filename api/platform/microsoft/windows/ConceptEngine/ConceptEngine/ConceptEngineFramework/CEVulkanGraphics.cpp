#include "CEVulkanGraphics.h"

#include "CEOSTools.h"
#include "CETools.h"
#include "CEVulkanManager.h"
#include "CEWindow.h"


CEVulkanGraphics::CEVulkanGraphics(HWND hWnd): CEGraphics(hWnd, CEOSTools::CEGraphicsApiTypes::vulkan),
                                               pVulkanData(std::make_unique<CEVulkanData>()), vulkanContext() {
	LoadVulkan();
	VkResult result;

	uint32_t layerCount = 0;
	pVulkanData->vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	VkLayerProperties* layersAvailable = new VkLayerProperties[layerCount];
	pVulkanData->vkEnumerateInstanceLayerProperties(&layerCount, layersAvailable);

	// Extensions:
	uint32_t extensionCount = 0;
	pVulkanData->vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	VkExtensionProperties* extensionsAvailable = new VkExtensionProperties[extensionCount];
	pVulkanData->vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionsAvailable);

	const char* extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_utils"};
	uint32_t numberRequiredExtensions = sizeof(extensions) / sizeof(char*);
	uint32_t foundExtensions = 0;
	for (uint32_t i = 0; i < extensionCount; ++i) {
		for (int j = 0; j < numberRequiredExtensions; ++j) {
			if (strcmp(extensionsAvailable[i].extensionName, extensions[j]) == 0) {
				foundExtensions++;
			}
		}
	}
	CETools::Assert(foundExtensions == numberRequiredExtensions, "Could not find debug extension");
	VkInstance instance;
	VkSurfaceKHR surface;
	//TODO: Load vulkan libraries dynamically
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Concept Engine";
	applicationInfo.engineVersion = 1;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.enabledExtensionCount = 3;
	instanceInfo.ppEnabledExtensionNames = extensions;

	result = pVulkanData->vkCreateInstance(&instanceInfo, NULL, &vulkanContext.instance);
	checkVulkanResult(result, "Failed to create vulkan instance.");

	LoadVulkanExtensions(vulkanContext);

	VkDebugUtilsMessengerCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	callbackCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	callbackCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// callbackCreateInfo.pfnUserCallback = &MyDebugReportCallback;

	result = pVulkanData->vkCreateDebugUtilsMessengerEXT(vulkanContext.instance, &callbackCreateInfo, nullptr,
	                                                     &vulkanContext.callback);
	checkVulkanResult(result, "Failed to create debug report callback.");

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	surfaceCreateInfo.hwnd = hWnd;

	result = pVulkanData->vkCreateWin32SurfaceKHR(vulkanContext.instance, &surfaceCreateInfo, nullptr,
	                                              &vulkanContext.surface);
	checkVulkanResult(result, "Could not create surface.");

	// Find a physical device that has a queue where we can present and do graphics: 
	uint32_t physicalDeviceCount = 0;
	pVulkanData->vkEnumeratePhysicalDevices(vulkanContext.instance, &physicalDeviceCount, nullptr);
	auto physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
	pVulkanData->vkEnumeratePhysicalDevices(vulkanContext.instance, &physicalDeviceCount, physicalDevices);

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

void CEVulkanGraphics::LoadVulkan() const {
	HMODULE vulkan_module = LoadLibrary(CETools::ConvertCharArrayToLPCWSTR("vulkan-1.dll"));
	pVulkanData->vkCreateInstance = (PFN_vkCreateInstance)GetProcAddress(vulkan_module, "vkCreateInstance");
	pVulkanData->vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)GetProcAddress(
		vulkan_module, "vkEnumerateInstanceLayerProperties");
	pVulkanData->vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)GetProcAddress(
		vulkan_module, "vkEnumerateInstanceExtensionProperties");
	pVulkanData->vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(
		vulkan_module, "vkGetInstanceProcAddr");
	pVulkanData->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)GetProcAddress(
		vulkan_module, "vkEnumeratePhysicalDevices");
	pVulkanData->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)GetProcAddress(
		vulkan_module, "vkGetPhysicalDeviceProperties");
	pVulkanData->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
		GetProcAddress(vulkan_module, "vkGetPhysicalDeviceQueueFamilyProperties");
	pVulkanData->vkCreateDevice = (PFN_vkCreateDevice)GetProcAddress(vulkan_module, "vkCreateDevice");
	pVulkanData->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)GetProcAddress(vulkan_module, "vkGetDeviceQueue");
	pVulkanData->vkCreateCommandPool = (PFN_vkCreateCommandPool)GetProcAddress(vulkan_module, "vkCreateCommandPool");
	pVulkanData->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)GetProcAddress(
		vulkan_module, "vkAllocateCommandBuffers");
	pVulkanData->vkCreateFence = (PFN_vkCreateFence)GetProcAddress(vulkan_module, "vkCreateFence");
	pVulkanData->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)GetProcAddress(vulkan_module, "vkBeginCommandBuffer");
	pVulkanData->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)GetProcAddress(vulkan_module, "vkCmdPipelineBarrier");
	pVulkanData->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)GetProcAddress(vulkan_module, "vkEndCommandBuffer");
	pVulkanData->vkQueueSubmit = (PFN_vkQueueSubmit)GetProcAddress(vulkan_module, "vkQueueSubmit");
	pVulkanData->vkWaitForFences = (PFN_vkWaitForFences)GetProcAddress(vulkan_module, "vkWaitForFences");
	pVulkanData->vkResetFences = (PFN_vkResetFences)GetProcAddress(vulkan_module, "vkResetFences");
	pVulkanData->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)GetProcAddress(vulkan_module, "vkResetCommandBuffer");
	pVulkanData->vkCreateImageView = (PFN_vkCreateImageView)GetProcAddress(vulkan_module, "vkCreateImageView");
	pVulkanData->vkCreateImage = (PFN_vkCreateImage)GetProcAddress(vulkan_module, "vkCreateImage");
	pVulkanData->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)GetProcAddress(
		vulkan_module, "vkGetImageMemoryRequirements");
	pVulkanData->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)GetProcAddress(
		vulkan_module, "vkGetPhysicalDeviceMemoryProperties");
	pVulkanData->vkAllocateMemory = (PFN_vkAllocateMemory)GetProcAddress(vulkan_module, "vkAllocateMemory");
	pVulkanData->vkBindImageMemory = (PFN_vkBindImageMemory)GetProcAddress(vulkan_module, "vkBindImageMemory");
	pVulkanData->vkCreateRenderPass = (PFN_vkCreateRenderPass)GetProcAddress(vulkan_module, "vkCreateRenderPass");
	pVulkanData->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)GetProcAddress(vulkan_module, "vkCreateFramebuffer");
	pVulkanData->vkCreateBuffer = (PFN_vkCreateBuffer)GetProcAddress(vulkan_module, "vkCreateBuffer");
	pVulkanData->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)GetProcAddress(
		vulkan_module, "vkGetBufferMemoryRequirements");
	pVulkanData->vkMapMemory = (PFN_vkMapMemory)GetProcAddress(vulkan_module, "vkMapMemory");
	pVulkanData->vkUnmapMemory = (PFN_vkUnmapMemory)GetProcAddress(vulkan_module, "vkUnmapMemory");
	pVulkanData->vkBindBufferMemory = (PFN_vkBindBufferMemory)GetProcAddress(vulkan_module, "vkBindBufferMemory");
	pVulkanData->vkCreateShaderModule = (PFN_vkCreateShaderModule)GetProcAddress(vulkan_module, "vkCreateShaderModule");
	pVulkanData->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)GetProcAddress(
		vulkan_module, "vkCreatePipelineLayout");
	pVulkanData->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)GetProcAddress(
		vulkan_module, "vkCreateGraphicsPipelines");
	pVulkanData->vkCreateSemaphore = (PFN_vkCreateSemaphore)GetProcAddress(vulkan_module, "vkCreateSemaphore");
	pVulkanData->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)GetProcAddress(vulkan_module, "vkCmdBeginRenderPass");
	pVulkanData->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)GetProcAddress(vulkan_module, "vkCmdBindPipeline");
	pVulkanData->vkCmdSetViewport = (PFN_vkCmdSetViewport)GetProcAddress(vulkan_module, "vkCmdSetViewport");
	pVulkanData->vkCmdSetScissor = (PFN_vkCmdSetScissor)GetProcAddress(vulkan_module, "vkCmdSetScissor");
	pVulkanData->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)GetProcAddress(
		vulkan_module, "vkCmdBindVertexBuffers");
	pVulkanData->vkCmdDraw = (PFN_vkCmdDraw)GetProcAddress(vulkan_module, "vkCmdDraw");
	pVulkanData->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)GetProcAddress(vulkan_module, "vkCmdEndRenderPass");
	pVulkanData->vkDestroyFence = (PFN_vkDestroyFence)GetProcAddress(vulkan_module, "vkDestroyFence");
	pVulkanData->vkDestroySemaphore = (PFN_vkDestroySemaphore)GetProcAddress(vulkan_module, "vkDestroySemaphore");
	pVulkanData->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)GetProcAddress(
		vulkan_module, "vkCreateDescriptorSetLayout");
	pVulkanData->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)GetProcAddress(
		vulkan_module, "vkCreateDescriptorPool");
	pVulkanData->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)GetProcAddress(
		vulkan_module, "vkAllocateDescriptorSets");
	pVulkanData->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)GetProcAddress(
		vulkan_module, "vkUpdateDescriptorSets");
	pVulkanData->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)GetProcAddress(
		vulkan_module, "vkCmdBindDescriptorSets");
	pVulkanData->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)GetProcAddress(
		vulkan_module, "vkFlushMappedMemoryRanges");
	pVulkanData->vkCreateSampler = (PFN_vkCreateSampler)GetProcAddress(vulkan_module, "vkCreateSampler");
}

void CEVulkanGraphics::LoadVulkanExtensions(CEVulkanContext context) {

	*(void**)&pVulkanData->vkCreateDebugUtilsMessengerEXT = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkCreateDebugUtilsMessengerEXT");
	*(void**)&pVulkanData->vkDestroyDebugUtilsMessengerEXT = pVulkanData->vkGetInstanceProcAddr(context.instance,
		"vkDestroyDebugUtilsMessengerEXT");

	*(void**)&pVulkanData->vkCreateWin32SurfaceKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkCreateWin32SurfaceKHR");
	*(void**)&pVulkanData->vkGetPhysicalDeviceSurfaceSupportKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	*(void**)&pVulkanData->vkGetPhysicalDeviceSurfaceFormatsKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	*(void**)&pVulkanData->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	*(void**)&pVulkanData->vkGetPhysicalDeviceSurfacePresentModesKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	*(void**)&pVulkanData->vkCreateSwapchainKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkCreateSwapchainKHR");
	*(void**)&pVulkanData->vkGetSwapchainImagesKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkGetSwapchainImagesKHR");
	*(void**)&pVulkanData->vkAcquireNextImageKHR = pVulkanData->vkGetInstanceProcAddr(
		context.instance, "vkAcquireNextImageKHR");
	*(void**)&pVulkanData->vkQueuePresentKHR = pVulkanData->
		vkGetInstanceProcAddr(context.instance, "vkQueuePresentKHR");

}

void CEVulkanGraphics::checkVulkanResult(VkResult& result, const char* msg) {
	CETools::Assert(result == VK_SUCCESS, msg);
}
