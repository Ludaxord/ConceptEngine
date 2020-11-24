#include "CEGraphics.h"
#pragma comment(lib, "d3d11.lib")
#include <sstream>
#include "dxerr.h"
#include "CEConverters.h"

#define GFX_THROW_INFO(hResultCall) if (FAILED(hResult = (hResultCall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hResultCall)
#define GFX_EXCEPT_NOINFO(hResult) CEGraphics::HResultException( __LINE__,__FILE__,(hResult) )
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hResult = (hrcall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hResult)
#ifndef NDEBUG
#define GFX_EXCEPT(hResult) CEGraphics::HResultException( __LINE__, __FILE__, (hResult), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hResult = (hrcall))) throw GFX_EXCEPT(hResult)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, (hResult), infoManager.GetMessages())
#else
#define GFX_EXCEPT(hResult) CEGraphics::HResultException(__LINE__, __FILE__, (hResult))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, hResult)
#endif

CEGraphics::HResultException::HResultException(int line, const char* file,
                                               HRESULT hResult,
                                               std::vector<std::string> infoMsgs) noexcept: CEException(line, file),
	hResult(hResult) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* CEGraphics::HResultException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec <<
		" (" << (unsigned long)GetErrorCode() << ")" << std::endl << "[Error String] " << GetErrorMessage() << std::endl
		<< "[Error Description] " << GetErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info] \n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
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

std::string CEGraphics::HResultException::GetErrorInfo() const noexcept {
	return info;
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
	UINT swapCreateFlags = 0u;

#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hResult;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
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
	GFX_THROW_INFO(pSwap->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(&pBackBuffer)
	));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
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
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hResult = pSwap->Present(1u, 0u))) {
		if (hResult == DXGI_ERROR_DEVICE_REMOVED) {
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else {
			throw GFX_EXCEPT(hResult);
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
