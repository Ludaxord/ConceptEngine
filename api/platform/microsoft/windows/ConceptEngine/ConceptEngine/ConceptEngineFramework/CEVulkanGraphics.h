#pragma once
#include "CEGraphics.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "CEVulkan.h"

class CEVulkanGraphics : public CEGraphics {
public:
	CEVulkanGraphics(HWND hWnd, int width, int height);
	void PrintGraphicsVersion() override;
	void LoadVulkan() const;
	void LoadVulkanExtensions(CEVulkanContext context);

private:
	static void checkVulkanResult(VkResult& result, const char* msg);

public:
	void OnRender() override;
	void OnUpdate() override;
	void SetFullscreen(bool fullscreen) override;
	bool OnInit() override;
	void OnDestroy() override;
	bool LoadContent() override;
	void UnloadContent() override;
protected:
	void OnKeyPressed() override;
	void OnKeyReleased() override;
	void OnMouseMoved() override;
	void OnMouseButtonPressed() override;
	void OnMouseButtonReleased() override;
	void OnMouseWheel() override;
	void OnResize() override;
	void OnWindowDestroy() override;
private:
	std::unique_ptr <CEVulkanData> pVulkanData;
	CEVulkanContext vulkanContext;

};
