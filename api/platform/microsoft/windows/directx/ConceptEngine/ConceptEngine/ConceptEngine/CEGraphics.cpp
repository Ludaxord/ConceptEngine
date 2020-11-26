#include "CEGraphics.h"

#include <sstream>
#include <d3dcompiler.h>
#include "dxerr.h"
#include "CEConverters.h"
namespace wrl = Microsoft::WRL;
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXCEPT_NOINFO(hResult) CEGraphics::HResultException( __LINE__,__FILE__,(hResult) )
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hResult = (hrcall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hResult)
#ifndef NDEBUG
#define GFX_EXCEPT(hResult) CEGraphics::HResultException( __LINE__, __FILE__, (hResult), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hResult = (hrcall))) throw GFX_EXCEPT(hResult)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, (hResult), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw CEGraphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hResult) CEGraphics::HResultException(__LINE__, __FILE__, (hResult))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, hResult)
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

CEGraphics::HResultException::HResultException(int line, const char* file,
                                               HRESULT hResult,
                                               std::vector<std::string> infoMsgs) noexcept: Exception(line, file),
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

CEGraphics::InfoException::InfoException(int line, const char* file,
                                         std::vector<std::string> infoMsgs) noexcept: Exception(line, file) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}

}

const char* CEGraphics::InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* CEGraphics::InfoException::GetType() const noexcept {
	return "Concept Engine Graphics Info Exception";
}

std::string CEGraphics::InfoException::GetErrorInfo() const noexcept {
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
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		&pBackBuffer
	));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));
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
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void CEGraphics::DrawDefaultTriangle() {
	namespace wrl = Microsoft::WRL;
	HRESULT hResult;
	const CEVertex vertices[] = {
		{0.0f, 0.5f, 255, 0, 0, 255},
		{0.5f, -0.5f, 0, 255, 0, 128},
		{-0.5f, -0.5f, 0, 0, 255, 0},
		{-0.3f, 0.3f, 0, 0, 255, 0},
		{0.0f, -0.8f, 255, 0, 0, 255},
	};

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(CEVertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	const UINT stride = sizeof(CEVertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	const unsigned short indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{
			"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			// or 8u,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};


	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"CEPixelShader.cso", &pBlob));
	GFX_THROW_INFO(
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"CEVertexShader.cso", &pBlob));
	GFX_THROW_INFO(
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	GFX_THROW_INFO(
		pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &
			pInputLayout));

	pContext->IASetInputLayout(pInputLayout.Get());

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		// D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		// D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	);

	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext-> DrawIndexed((UINT)std::size(indices),0u,0u) );
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
