#include "CEDirectXHandlers.h"

#include <dxgi1_3.h>

using namespace ConceptEngine::Graphics::DirectX12::Base;

PFN_CREATE_DXGI_FACTORY_2 CreateDXGIFactory2Func = nullptr;
PFN_DXGI_GET_DEBUG_INTERFACE_1 DXGIGetDebugInterface1Func = nullptr;
PFN_D3D12_CREATE_DEVICE D3D12CreateDeviceFunc = nullptr;
PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterfaceFunc = nullptr;
PFN_D3D12_SERIALIZE_ROOT_SIGNATURE D3D12SerializeRootSignatureFunc = nullptr;
PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER D3D12CreateRootSignatureDeserializerFunc = nullptr;
PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE D3D12SerializeVersionedRootSignatureFunc = nullptr;
PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER D3D12CreateVersionedRootSignatureDeserializerFunc = nullptr;
PFN_SetMarkerOnCommandList SetMarkerOnCommandListFunc = nullptr;

CEDirectXHandler* DXHandler = nullptr;

//LIB
CEDirectXLibHandler::CEDirectXLibHandler(): CEDirectXHandler() {
}

bool CEDirectXLibHandler::Create() {
	return true;
}

HRESULT CEDirectXLibHandler::XCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) {
	const HRESULT hr = CreateDXGIFactory2(Flags, IID_PPV_ARGS(&ppFactory));
	return hr;
}

HRESULT CEDirectXLibHandler::XDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) {
	const HRESULT hr = DXGIGetDebugInterface1(Flags, IID_PPV_ARGS(&dxgiDebug));
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
                                                ID3D12Device* device) {
	const HRESULT hr = D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&device));
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) {
	const HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&dxgiDebug));
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
                                                          D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
                                                          ID3DBlob** errorBlob) {
	const HRESULT hr = D3D12SerializeRootSignature(rootSignatureDesc, version, blob, errorBlob);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                   const IID& pRootSignatureDeserializerInterface,
                                                                   void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                        pRootSignatureDeserializerInterface,
	                                                        ppRootSignatureDeserializer);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                            const IID&
                                                                            pRootSignatureDeserializerInterface,
                                                                            void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateVersionedRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                                 pRootSignatureDeserializerInterface,
	                                                                 ppRootSignatureDeserializer);
	return hr;
}

void CEDirectXLibHandler::XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
                                                  PCSTR formatString) {
	PIXSetMarker(commandList, color, formatString);
}

bool CEDirectXLibHandler::LibCreate() {
	DXHandler = new CEDirectXLibHandler();
	DXHandler->Create();

	return true;
}

void CEDirectXLibHandler::LibDestroy() {
	DXHandler->Destroy();
	delete DXHandler;
	DXHandler = nullptr;
}

void CEDirectXLibHandler::Destroy() {
}

HRESULT CEDirectXLibHandler::LibCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) {
	return DXHandler->XCreateDXGIFactory2(Flags, ppFactory);
}

HRESULT CEDirectXLibHandler::LibDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) {
	return DXHandler->XDXGIGetDebugInterface1(Flags, dxgiDebug);
}

HRESULT CEDirectXLibHandler::LibD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
                                                  ID3D12Device* device) {
	return DXHandler->XD3D12CreateDevice(adapter, featureLevel, device);
}

HRESULT CEDirectXLibHandler::LibD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) {
	return DXHandler->XD3D12GetDebugInterface(dxgiDebug);
}

HRESULT CEDirectXLibHandler::LibD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
                                                            D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
                                                            ID3DBlob** errorBlob) {
	return DXHandler->XD3D12SerializeRootSignature(rootSignatureDesc, version, blob, errorBlob);
}

HRESULT CEDirectXLibHandler::LibD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                     const IID& pRootSignatureDeserializerInterface,
                                                                     void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                        pRootSignatureDeserializerInterface,
	                                                        ppRootSignatureDeserializer);
}

HRESULT CEDirectXLibHandler::LibD3D12CreateVersionedRootSignatureDeserializer(
	LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
	const IID& pRootSignatureDeserializerInterface,
	void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateVersionedRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                                 pRootSignatureDeserializerInterface,
	                                                                 ppRootSignatureDeserializer);
}

void CEDirectXLibHandler::LibSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
                                                    PCSTR formatString) {
	return DXHandler->XSetMarkerOnCommandList(commandList, color, formatString);
}

//DLL
CEDirectXDLLHandler::CEDirectXDLLHandler(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib): CEDirectXHandler(),
	DXGILib(DXGILib), D3D12Lib(D3D12Lib), PIXLib(PIXLib) {
}

bool CEDirectXDLLHandler::Create() {
	CreateDXGIFactory2Func = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_CREATE_DXGI_FACTORY_2>(DXGILib, "CreateDXGIFactory2");

	DXGIGetDebugInterface1Func = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_DXGI_GET_DEBUG_INTERFACE_1>(DXGILib, "DXGIGetDebugInterface1");

	D3D12CreateDeviceFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_D3D12_CREATE_DEVICE>(D3D12Lib, "D3D12CreateDevice");

	D3D12GetDebugInterfaceFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_D3D12_GET_DEBUG_INTERFACE>(D3D12Lib, "D3D12GetDebugInterface");

	D3D12SerializeRootSignatureFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_D3D12_SERIALIZE_ROOT_SIGNATURE>(D3D12Lib, "D3D12SerializeRootSignature");

	D3D12SerializeVersionedRootSignatureFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE>(
		D3D12Lib,
		"D3D12SerializeVersionedRootSignature");
	D3D12CreateRootSignatureDeserializerFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
		PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER>(
		D3D12Lib,
		"D3D12CreateRootSignatureDeserializer");
	D3D12CreateVersionedRootSignatureDeserializerFunc =
		Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
			PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER>(
			D3D12Lib,
			"D3D12CreateVersionedRootSignatureDeserializer");

	if (PIXLib != NULL) {
		CE_LOG_INFO("Loaded WinPixEventRuntime.dll");
		SetMarkerOnCommandListFunc = Core::Platform::Windows::Actions::CEWindowsActions::GetTypedProcAddress<
			PFN_SetMarkerOnCommandList>(
			PIXLib, "PIXSetMarkerOnCommandList");
	}
	return true;
}

void CEDirectXDLLHandler::Destroy() {
	if (DXGILib) {
		::FreeLibrary(DXGILib);
		DXGILib = 0;
	}

	if (D3D12Lib) {
		::FreeLibrary(D3D12Lib);
		D3D12Lib = 0;
	}
}

void CEDirectXDLLHandler::DLLDestroy() {
	DXHandler->Destroy();
	delete DXHandler;
	DXHandler = nullptr;
}

HRESULT CEDirectXDLLHandler::XCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) {
	const HRESULT hr = CreateDXGIFactory2Func(Flags, IID_PPV_ARGS(&ppFactory));
	return hr;
}

HRESULT CEDirectXDLLHandler::XDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) {
	const HRESULT hr = DXGIGetDebugInterface1Func(Flags, IID_PPV_ARGS(&dxgiDebug));
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
                                                ID3D12Device* device) {
	const HRESULT hr = D3D12CreateDeviceFunc(adapter, featureLevel, IID_PPV_ARGS(&device));
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) {
	const HRESULT hr = D3D12GetDebugInterfaceFunc(IID_PPV_ARGS(&dxgiDebug));
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
                                                          D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
                                                          ID3DBlob** errorBlob) {
	const HRESULT hr = D3D12SerializeRootSignatureFunc(rootSignatureDesc, version, blob, errorBlob);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                   const IID& pRootSignatureDeserializerInterface,
                                                                   void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateRootSignatureDeserializerFunc(pSrcData, SrcDataSizeInBytes,
	                                                            pRootSignatureDeserializerInterface,
	                                                            ppRootSignatureDeserializer);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                            const IID&
                                                                            pRootSignatureDeserializerInterface,
                                                                            void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateVersionedRootSignatureDeserializerFunc(pSrcData, SrcDataSizeInBytes,
	                                                                     pRootSignatureDeserializerInterface,
	                                                                     ppRootSignatureDeserializer);
	return hr;
}

void CEDirectXDLLHandler::XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
                                                  PCSTR formatString) {
	SetMarkerOnCommandListFunc(commandList, color, formatString);
}

bool CEDirectXDLLHandler::DLLCreate(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib) {
	DXHandler = new CEDirectXDLLHandler(DXGILib, D3D12Lib, PIXLib);
	DXHandler->Create();

	return true;
}

HRESULT CEDirectXDLLHandler::DLLCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) {
	return DXHandler->XCreateDXGIFactory2(Flags, ppFactory);
}

HRESULT CEDirectXDLLHandler::DLLDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) {
	return DXHandler->XDXGIGetDebugInterface1(Flags, dxgiDebug);
}

HRESULT CEDirectXDLLHandler::DLLD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
                                                  ID3D12Device* device) {
	return DXHandler->XD3D12CreateDevice(adapter, featureLevel, device);
}

HRESULT CEDirectXDLLHandler::DLLD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) {
	return DXHandler->XD3D12GetDebugInterface(dxgiDebug);
}

HRESULT CEDirectXDLLHandler::DLLD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
                                                            D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
                                                            ID3DBlob** errorBlob) {
	return DXHandler->XD3D12SerializeRootSignature(rootSignatureDesc, version, blob, errorBlob);
}

HRESULT CEDirectXDLLHandler::DLLD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
                                                                     const IID& pRootSignatureDeserializerInterface,
                                                                     void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                        pRootSignatureDeserializerInterface,
	                                                        ppRootSignatureDeserializer);
}

HRESULT CEDirectXDLLHandler::DLLD3D12CreateVersionedRootSignatureDeserializer(
	LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
	const IID&
	pRootSignatureDeserializerInterface,
	void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateVersionedRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                                 pRootSignatureDeserializerInterface,
	                                                                 ppRootSignatureDeserializer);
}

void CEDirectXDLLHandler::DLLSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
                                                    PCSTR formatString) {
	DXHandler->XSetMarkerOnCommandList(commandList, color, formatString);
}
