#include "CEMetalGraphics.h"

#include "CEMetalManager.h"
#include "CEOSTools.h"

CEMetalGraphics::CEMetalGraphics(HWND hWnd): CEGraphics(hWnd, CEOSTools::CEGraphicsApiTypes::metal) {
}

CEGraphicsManager CEMetalGraphics::GetGraphicsManager() {
	return static_cast<CEGraphicsManager>(CEMetalManager());
}

void CEMetalGraphics::PrintGraphicsVersion() {
}
