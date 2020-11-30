#include "CEGraphics.h"

#include <sstream>
#include <d3dcompiler.h>
#include <iostream>

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

CEGraphics::CEGraphics(HWND hWnd, CEGraphicsApiTypes apiType) {
	graphicsApiType = apiType;

	ResolveSelectedGraphicsAPI();

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

	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = TRUE;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;

	GFX_THROW_INFO(pDevice->CreateDepthStencilState(
		&ds_desc,
		&pDSState
	));

	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descStencilView = {};
	descStencilView.Format = DXGI_FORMAT_D32_FLOAT;
	descStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descStencilView.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(
		pDepthStencil.Get(),
		&descStencilView,
		&pDepthStencilView
	));

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepthStencilView.Get());
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
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void CEGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
                                   CEDefaultFigureTypes figureTypes) {

	HRESULT hResult;

	//TODO: find way to change array of CEVertex to vector of CEVertex without manipulate with CEVertex size.
	CEVertex vertices[] = {
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	//TODO: Fix
	std::vector<CEVertex> verticesVector;
	if (figureTypes == CEDefaultFigureTypes::triangle2d) {
	}
	else if (figureTypes == CEDefaultFigureTypes::cube3d) {
	}
	// CEVertex* vertices = &verticesVector[0];
	// CEVertex* vertices = verticesVector.data();
	/////


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

	//TODO: dynamic change of indices when different types of vertex is passed
	const unsigned short indices[] = {
		0, 2, 1, 2, 3, 1,
		1, 3, 5, 3, 7, 5,
		2, 6, 3, 3, 6, 7,
		4, 5, 7, 4, 7, 6,
		0, 4, 2, 2, 4, 6,
		0, 1, 4, 1, 5, 4
	};
	//TODO: Fix
	std::vector<unsigned short> indicesVector;
	if (figureTypes == CEDefaultFigureTypes::triangle2d) {
		indicesVector = {
			0, 1, 2,
			0, 2, 3,
			0, 4, 1,
			2, 1, 5
		};
	}
	else if (figureTypes == CEDefaultFigureTypes::cube3d) {
		indicesVector = {};
	}
	/////

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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const auto aspectRatio = CEConverters::gcd((int)windowWidth, (int)windowHeight);
	const auto aspectRatioWidth = windowWidth / aspectRatio;
	const auto aspectRatioHeight = windowHeight / aspectRatio;

	const CEConstantBuffer cb = GetDefaultConstantBuffer(angle, aspectRatioWidth, aspectRatioHeight, x, y, z);
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	auto cfcb = GetDefaultFaceColorsConstantBuffer();
	wrl::ComPtr<ID3D11Buffer> pColorFacedConstantBuffer;
	D3D11_BUFFER_DESC cfcbd;
	cfcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cfcbd.Usage = D3D11_USAGE_DYNAMIC;
	cfcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cfcbd.MiscFlags = 0u;
	cfcbd.ByteWidth = sizeof(cfcb);
	cfcbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA cfcsd = {};
	cfcsd.pSysMem = &cfcb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cfcbd, &cfcsd, &pColorFacedConstantBuffer));

	pContext->PSSetConstantBuffers(0u, 1u, pColorFacedConstantBuffer.GetAddressOf());

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

	// pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		// D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		// D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	);

	D3D11_VIEWPORT vp;
	vp.Width = windowWidth;
	vp.Height = windowHeight;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext-> DrawIndexed((UINT)std::size(indices),0u,0u));
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

void CEGraphics::ResolveSelectedGraphicsAPI() {
	std::ostringstream oss;
	oss << "Graphics API type: ";
	//TODO: Make CEGraphics Virtual and create implementation for every API -> Direct3D 11, Direct3D 12, Vulkan, OpenGL
	switch (graphicsApiType) {
	case CEGraphicsApiTypes::direct3d11:
		oss << "Direct3D 11";
		break;
	case CEGraphicsApiTypes::direct3d12:
		oss << "Direct3D 12";
		break;
	case CEGraphicsApiTypes::vulkan:
		oss << "Vulkan";
		break;
	case CEGraphicsApiTypes::opengl:
		oss << "OpenGL";
		break;
	default:
		oss << "Direct3D 11 - Default";
		break;
	}
	oss << std::endl;
	OutputDebugString(CEConverters::ConvertCharArrayToLPCWSTR(oss.str().c_str()));
}
