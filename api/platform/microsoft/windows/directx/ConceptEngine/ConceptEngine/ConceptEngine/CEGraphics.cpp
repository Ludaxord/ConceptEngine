#include "CEGraphics.h"
#pragma comment(lib, "d3d11.lib")
#include <sstream>
#include "dxerr.h"
#include "CEConverters.h"

#define GFX_THROW_FAILED(hResultCall) if (FAILED(hResult = (hResultCall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hResultCall)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, hResult)

CEGraphics::HResultException::HResultException(int line, const char* file,
                                               HRESULT hResult) noexcept: CEException(line, file), hResult(hResult) {
}

const char* CEGraphics::HResultException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec <<
		" (" << (unsigned long)GetErrorCode() << ")" << std::endl << "[Error String] " << GetErrorMessage() << std::endl
		<< "[Error Description] " << GetErrorDescription() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* CEGraphics::HResultException::GetType() const noexcept {
	return "Concept Engine Graphics Exception";
}

HRESULT CEGraphics::HResultException::GetErrorCode() const noexcept {
	return hResult;
}

std::string CEGraphics::HResultException::GetErrorMessage() const noexcept {
	std::wstring ws(DXGetErrorString(hResult));
	std::string errorMessage(ws.begin(), ws.end());
	return errorMessage;
}

std::string CEGraphics::HResultException::GetErrorDescription() const noexcept {
	char buff[512];
	DXGetErrorDescription(hResult, CEConverters::ConvertCharArrayToLPCWSTR(buff), sizeof(buff));
	return buff;
}

const char* CEGraphics::DeviceRemovedException::GetType() const noexcept {
	return "Concept Engine Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

CEGraphics::CEGraphics(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC sd = GetDefaultD311Descriptor(hWnd);

	HRESULT hResult;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED(pSwap->GetBuffer(
		0,
		// __uuidof(ID3D11Resource),
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	));
	pBackBuffer->Release();
}

CEGraphics::~CEGraphics() {
	if (pTarget != nullptr) {
		pTarget->Release();
	}
	if (pContext != nullptr) {
		pContext->Release();
	}
	if (pSwap != nullptr) {
		pSwap->Release();
	}
	if (pDevice != nullptr) {
		pDevice->Release();
	}
}

void CEGraphics::EndFrame() {
	HRESULT hResult;
	if (FAILED(hResult = pSwap->Present(1u, 0u))) {
		if (hResult == DXGI_ERROR_DEVICE_REMOVED) {
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else {
			GFX_THROW_FAILED(hResult);
		}
	}
}

void CEGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
	const float color[] = {red, green, blue, 1.0f};
	pContext->ClearRenderTargetView(pTarget, color);
}

DXGI_SWAP_CHAIN_DESC CEGraphics::GetDefaultD311Descriptor(HWND hWnd) noexcept {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	return sd;
}
