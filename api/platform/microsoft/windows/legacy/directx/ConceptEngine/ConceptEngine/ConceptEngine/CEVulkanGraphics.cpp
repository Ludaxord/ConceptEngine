#include "CEVulkanGraphics.h"

CEVulkanGraphics::CEVulkanGraphics(HWND hWnd): CEGraphics(hWnd, CEGraphicsApiTypes::vulkan) {
}

void CEVulkanGraphics::EndFrame() {
}

void CEVulkanGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEVulkanGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	CEDefaultFigureTypes figureTypes) {
}
