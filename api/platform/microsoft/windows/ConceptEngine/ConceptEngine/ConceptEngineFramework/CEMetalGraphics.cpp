#include "CEMetalGraphics.h"

#include "CEOSTools.h"
#include "CETools.h"
#include "CEWindow.h"

CEMetalGraphics::CEMetalGraphics(HWND hWnd, int width, int height): CEGraphics(
	hWnd, CEOSTools::CEGraphicsApiTypes::metal, width, height) {
}

void CEMetalGraphics::PrintGraphicsVersion() {
}

bool CEMetalGraphics::OnInit() {
	return true;
}

void CEMetalGraphics::OnDestroy() {
}

void CEMetalGraphics::OnRender() {
}

void CEMetalGraphics::OnUpdate() {
}

bool CEMetalGraphics::LoadContent() {
	return false;
}

void CEMetalGraphics::UnloadContent() {
}

void CEMetalGraphics::UpdatePerSecond(float second) {
}

CEGraphics::IGPUInfo CEMetalGraphics::GetGPUInfo() {
	return IGPUInfo();
}

void CEMetalGraphics::OnKeyPressed() {
}

void CEMetalGraphics::OnKeyReleased() {
}

void CEMetalGraphics::OnMouseMoved() {
}

void CEMetalGraphics::OnMouseButtonPressed() {
}

void CEMetalGraphics::OnMouseButtonReleased() {
}

void CEMetalGraphics::OnMouseWheel() {
}

void CEMetalGraphics::OnResize() {
}

void CEMetalGraphics::OnWindowDestroy() {
}

void CEMetalGraphics::SetFullscreen(bool fullscreen) {
}
