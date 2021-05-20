#pragma once
#include "../RenderLayer/CEDXHelper.h"

typedef HRESULT (WINAPI* PFN_CREATE_DXGI_FACTORY_2)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
typedef HRESULT (WINAPI* PFN_DXGI_GET_DEBUG_INTERFACE_1)(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug);
typedef HRESULT (WINAPI* PFN_SetMarkerOnCommandList)(ID3D12GraphicsCommandList* commandList, UINT64 color,
                                                     _In_ PCSTR formatString);

extern PFN_CREATE_DXGI_FACTORY_2 CreateDXGIFactory2Func;
extern PFN_DXGI_GET_DEBUG_INTERFACE_1 DXGIGetDebugInterface1Func;
extern PFN_D3D12_CREATE_DEVICE D3D12CreateDeviceFunc;
extern PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterfaceFunc;
extern PFN_D3D12_SERIALIZE_ROOT_SIGNATURE D3D12SerializeRootSignatureFunc;
extern PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER D3D12CreateRootSignatureDeserializerFunc;
extern PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE D3D12SerializeVersionedRootSignatureFunc;
extern PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER D3D12CreateVersionedRootSignatureDeserializerFunc;
extern PFN_SetMarkerOnCommandList SetMarkerOnCommandListFunc;

namespace ConceptEngine::Graphics::DirectX12::Base {

	class CEDirectXHandler {
	public:
	protected:
	private:
	};
}
