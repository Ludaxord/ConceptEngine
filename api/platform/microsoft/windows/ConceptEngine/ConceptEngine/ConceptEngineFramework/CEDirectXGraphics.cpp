#include "CEDirectXGraphics.h"

#include <spdlog/spdlog.h>

CEDirectXGraphics::CEDirectXGraphics(HWND hWnd, int width, int height): CEGraphics(
	hWnd, CEOSTools::CEGraphicsApiTypes::direct3d12, width, height) {
}

bool CEDirectXGraphics::OnInit() {
	
	return false;
}

void CEDirectXGraphics::OnDestroy() {
}

void CEDirectXGraphics::OnRender() {
}

void CEDirectXGraphics::OnUpdate() {
}

void CEDirectXGraphics::OnResize() {
}

void CEDirectXGraphics::UpdatePerSecond(float second) {
}

bool CEDirectXGraphics::LoadContent() {
	return false;
}

void CEDirectXGraphics::UnloadContent() {
}

void CEDirectXGraphics::LoadBonus() {
}

void CEDirectXGraphics::InitGui() {
}

void CEDirectXGraphics::RenderGui() {
}

void CEDirectXGraphics::DestroyGui() {
}

void CEDirectXGraphics::OnKeyPressed() {
}

void CEDirectXGraphics::OnKeyReleased() {
}

void CEDirectXGraphics::OnMouseMoved() {
}

void CEDirectXGraphics::OnMouseButtonPressed() {
}

void CEDirectXGraphics::OnMouseButtonReleased() {
}

void CEDirectXGraphics::OnMouseWheel() {
}

void CEDirectXGraphics::OnWindowDestroy() {
}

void CEDirectXGraphics::SetFullscreen(bool fullscreen) {
}
