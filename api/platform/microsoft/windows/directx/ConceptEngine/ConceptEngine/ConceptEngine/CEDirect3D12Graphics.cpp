#include "CEDirect3D12Graphics.h"
namespace wrl = Microsoft::WRL;


void CEDirect3D12Graphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
                                             float z, CEDefaultFigureTypes figureTypes) {
}

CEDirect3D12Graphics::CEDirect3D12Graphics(HWND hWnd): CEGraphics(hWnd, CEGraphicsApiTypes::direct3d12) {
}

void CEDirect3D12Graphics::EndFrame() {
}

void CEDirect3D12Graphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {

}
