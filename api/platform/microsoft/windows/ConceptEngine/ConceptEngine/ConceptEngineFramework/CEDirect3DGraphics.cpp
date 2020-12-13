#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>


#include "CEDirect3D12Manager.h"
#include "CETools.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType): CEGraphics(hWnd, apiType) {
	if (apiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		CreateDirect3D11();
	}
	else if (apiType == CEOSTools::CEGraphicsApiTypes::direct3d12) {
		CreateDirect3D12();
	}
	else {
		std::ostringstream oss;
		oss << "No API for enum: ";
		oss << magic_enum::enum_name(apiType);
		oss << std::endl;
		throw CEException(__LINE__, oss.str().c_str());
	}
}

void CEDirect3DGraphics::EndFrame() {
#ifndef NDEBUG
	infoManager.Set();
#endif
}

void CEDirect3DGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEDirect3DGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
                                           float z, CEDefaultFigureTypes figureTypes) {
}

void CEDirect3DGraphics::CreateDirect3D12() {
}

void CEDirect3DGraphics::CreateDirect3D11() {
}

CEGraphicsManager CEDirect3DGraphics::GetGraphicsManager() {
	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		return static_cast<CEGraphicsManager>(CEDirect3D11Manager());
	} else {
		return static_cast<CEGraphicsManager>(CEDirect3D12Manager());
	}
}
