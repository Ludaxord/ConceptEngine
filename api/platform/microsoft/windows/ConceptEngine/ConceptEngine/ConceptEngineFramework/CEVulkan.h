#pragma once
#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>

struct CEShaderUniform {
	VkBuffer buffer;
	VkDeviceMemory memory;

	float* projectionMatrix;
	float* viewMatrix;
	float* modelMatrix;

};

struct CEVulkanData {
	PFN_vkCreateInstance vkCreateInstance = nullptr;
	PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkCreateDevice vkCreateDevice = nullptr;
	PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
	PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
	PFN_vkCreateFence vkCreateFence = nullptr;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
	PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
	PFN_vkQueueSubmit vkQueueSubmit = nullptr;
	PFN_vkWaitForFences vkWaitForFences = nullptr;
	PFN_vkResetFences vkResetFences = nullptr;
	PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
	PFN_vkCreateImageView vkCreateImageView = nullptr;
	PFN_vkCreateImage vkCreateImage = nullptr;
	PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkAllocateMemory vkAllocateMemory = nullptr;
	PFN_vkBindImageMemory vkBindImageMemory = nullptr;
	PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
	PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
	PFN_vkCreateBuffer vkCreateBuffer = nullptr;
	PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
	PFN_vkMapMemory vkMapMemory = nullptr;
	PFN_vkUnmapMemory vkUnmapMemory = nullptr;
	PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
	PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
	PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
	PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
	PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
	PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
	PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
	PFN_vkCmdDraw vkCmdDraw = nullptr;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;
	PFN_vkDestroyFence vkDestroyFence = nullptr;
	PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
	PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
	PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
	PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
	PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
	PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
	PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = nullptr;
	PFN_vkCreateSampler vkCreateSampler = nullptr;

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;

	// Windows platform:
	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

	// Swapchain extension:
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
	PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
};


struct CEVulkanContext {

	uint32_t width;
	uint32_t height;

	float cameraZ;
	int32_t cameraZDir;

	uint32_t presentQueueIdx;
	VkQueue presentQueue;

	VkInstance instance;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	VkImage* presentImages;
	VkImage depthImage;
	VkImageView depthImageView;
	VkFramebuffer* frameBuffers;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	VkDevice device;

	VkCommandBuffer setupCmdBuffer;
	VkCommandBuffer drawCmdBuffer;

	VkRenderPass renderPass;

	VkBuffer vertexInputBuffer;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSet;

	CEShaderUniform uniforms;

	VkDebugUtilsMessengerEXT callback;
};
