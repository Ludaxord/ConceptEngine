#pragma once
#include "CEGraphics.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "CEVulkan.h"

class CEVulkanGraphics : public CEGraphics {
public:
	CEVulkanGraphics(HWND hWnd);
	CEGraphicsManager GetGraphicsManager() override;
	void PrintGraphicsVersion() override;
	void LoadVulkan() const;
	void LoadVulkanExtensions(CEVulkanContext context);

private:
	static void checkVulkanResult(VkResult& result, const char* msg);

public:
	void OnRender() override;
	void OnUpdate() override;
	void SetFullscreen(bool fullscreen) override;
	void OnInit() override;
	void OnDestroy() override;
private:
	std::unique_ptr <CEVulkanData> pVulkanData;
	CEVulkanContext vulkanContext;

};
