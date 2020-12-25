#include "CEMetalGraphics.h"

#include "CEMetalManager.h"
#include "CEOSTools.h"
#include "CETools.h"
#include "CEWindow.h"

CEMetalGraphics::CEMetalGraphics(HWND hWnd): CEGraphics(hWnd, CEOSTools::CEGraphicsApiTypes::metal) {
}

void CEMetalGraphics::SetGraphicsManager() {
	int width, height;
	std::tie(width, height) = CETools::WindowSize(hWnd);
	auto manager = std::make_unique<CEMetalManager>(CETools::ConvertCharArrayToLPCWSTR(CEWindow::GetName()),
	                                                width,
	                                                height,
	                                                g_VSync);
	pManager = std::move(manager);
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
