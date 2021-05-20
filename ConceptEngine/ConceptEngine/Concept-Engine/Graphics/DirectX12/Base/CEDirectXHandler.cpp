#include "CEDirectXHandler.h"

using namespace ConceptEngine::Graphics::DirectX12::Base;

PFN_CREATE_DXGI_FACTORY_2                              CreateDXGIFactory2Func                            = nullptr;
PFN_DXGI_GET_DEBUG_INTERFACE_1                         DXGIGetDebugInterface1Func                        = nullptr;
PFN_D3D12_CREATE_DEVICE                                D3D12CreateDeviceFunc                             = nullptr;
PFN_D3D12_GET_DEBUG_INTERFACE                          D3D12GetDebugInterfaceFunc                        = nullptr;
PFN_D3D12_SERIALIZE_ROOT_SIGNATURE                     D3D12SerializeRootSignatureFunc                   = nullptr;
PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER           D3D12CreateRootSignatureDeserializerFunc          = nullptr;
PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE           D3D12SerializeVersionedRootSignatureFunc          = nullptr;
PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER D3D12CreateVersionedRootSignatureDeserializerFunc = nullptr;
PFN_SetMarkerOnCommandList                             SetMarkerOnCommandListFunc                        = nullptr;