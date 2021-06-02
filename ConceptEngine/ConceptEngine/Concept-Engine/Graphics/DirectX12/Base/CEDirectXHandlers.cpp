#include "CEDirectXHandlers.h"

#include "../../../Core/Debug/CEDebug.h"

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

HRESULT CEDirectXLibHandler::XCreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
	const HRESULT hr = CreateDXGIFactory2(Flags, riid, ppFactory);
	return hr;
}

HRESULT CEDirectXLibHandler::XDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) {
	const HRESULT hr = DXGIGetDebugInterface1(Flags, riid, pDebug);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
                                                D3D_FEATURE_LEVEL MinimumFeatureLevel,
                                                _In_ REFIID riid, // Expected: ID3D12Device
                                                _COM_Outptr_opt_ void** ppDevice) {
	const HRESULT hr = D3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) {
	const HRESULT hr = D3D12GetDebugInterface(riid, ppvDebug);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
                                                          _In_ D3D_ROOT_SIGNATURE_VERSION Version,
                                                          _Out_ ID3DBlob** ppBlob,
                                                          _Always_(_Outptr_opt_result_maybenull_) ID3DBlob**
                                                          ppErrorBlob) {
	const HRESULT hr = D3D12SerializeRootSignature(pRootSignature, Version, ppBlob, ppErrorBlob);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                        pRootSignatureDeserializerInterface,
	                                                        ppRootSignatureDeserializer);
	return hr;
}

HRESULT CEDirectXLibHandler::XD3D12CreateVersionedRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
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
	if (!DXHandler->Create()) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	} 

	return true;
}

void CEDirectXLibHandler::LibDestroy() {
	DXHandler->Destroy();
	delete DXHandler;
	DXHandler = nullptr;
}

void CEDirectXLibHandler::Destroy() {
}

bool CEDirectXHandler::CECreate(CreateOption option, HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib) {
	switch (option) {
	case CreateOption::Lib:
		return CEDirectXLibHandler::LibCreate();
	case CreateOption::DLL:
		return CEDirectXDLLHandler::DLLCreate(DXGILib, D3D12Lib, PIXLib);
	}
	return false;
}

void CEDirectXHandler::CEDestroy() {
}

HRESULT CEDirectXHandler::CECreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
	return DXHandler->XCreateDXGIFactory2(Flags, riid, ppFactory);
}

HRESULT CEDirectXHandler::CEDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) {
	return DXHandler->XDXGIGetDebugInterface1(Flags, riid, pDebug);
}

HRESULT CEDirectXHandler::CED3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
                                              D3D_FEATURE_LEVEL MinimumFeatureLevel,
                                              _In_ REFIID riid, // Expected: ID3D12Device
                                              _COM_Outptr_opt_ void** ppDevice) {
	return DXHandler->XD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
}

HRESULT CEDirectXHandler::CED3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) {
	return DXHandler->XD3D12GetDebugInterface(riid, ppvDebug);
}

HRESULT CEDirectXHandler::CED3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
                                                        _In_ D3D_ROOT_SIGNATURE_VERSION Version,
                                                        _Out_ ID3DBlob** ppBlob,
                                                        _Always_(_Outptr_opt_result_maybenull_) ID3DBlob**
                                                        ppErrorBlob) {
	return DXHandler->XD3D12SerializeRootSignature(pRootSignature, Version, ppBlob, ppErrorBlob);
}

HRESULT CEDirectXHandler::CED3D12CreateRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                        pRootSignatureDeserializerInterface,
	                                                        ppRootSignatureDeserializer);
}

HRESULT CEDirectXHandler::CED3D12CreateVersionedRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
	return DXHandler->XD3D12CreateVersionedRootSignatureDeserializer(pSrcData, SrcDataSizeInBytes,
	                                                                 pRootSignatureDeserializerInterface,
	                                                                 ppRootSignatureDeserializer);
}

void CEDirectXHandler::CESetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
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

HRESULT CEDirectXDLLHandler::XCreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) {
	const HRESULT hr = CreateDXGIFactory2Func(Flags, riid, ppFactory);
	return hr;
}

HRESULT CEDirectXDLLHandler::XDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) {
	const HRESULT hr = DXGIGetDebugInterface1Func(Flags, riid, pDebug);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
                                                D3D_FEATURE_LEVEL MinimumFeatureLevel,
                                                _In_ REFIID riid, // Expected: ID3D12Device
                                                _COM_Outptr_opt_ void** ppDevice) {
	const HRESULT hr = D3D12CreateDeviceFunc(pAdapter, MinimumFeatureLevel, riid, ppDevice);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) {
	const HRESULT hr = D3D12GetDebugInterfaceFunc(riid, ppvDebug);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
                                                          _In_ D3D_ROOT_SIGNATURE_VERSION Version,
                                                          _Out_ ID3DBlob** ppBlob,
                                                          _Always_(_Outptr_opt_result_maybenull_) ID3DBlob**
                                                          ppErrorBlob) {
	const HRESULT hr = D3D12SerializeRootSignatureFunc(pRootSignature, Version, ppBlob, ppErrorBlob);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
	const HRESULT hr = D3D12CreateRootSignatureDeserializerFunc(pSrcData, SrcDataSizeInBytes,
	                                                            pRootSignatureDeserializerInterface,
	                                                            ppRootSignatureDeserializer);
	return hr;
}

HRESULT CEDirectXDLLHandler::XD3D12CreateVersionedRootSignatureDeserializer(
	_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
	_In_ SIZE_T SrcDataSizeInBytes,
	_In_ REFIID pRootSignatureDeserializerInterface,
	_Out_ void** ppRootSignatureDeserializer) {
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
