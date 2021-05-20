#pragma once
#include "../RenderLayer/CEDXHelper.h"
#include "../../../Core/Platform/Windows/Actions/CEWindowsActions.h"

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
		CEDirectXHandler() = default;
		virtual ~CEDirectXHandler() = default;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;
		virtual HRESULT XCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) = 0;
		virtual HRESULT XDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) = 0;
		virtual HRESULT XD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
		                                   ID3D12Device* device) = 0;
		virtual HRESULT XD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) = 0;
		virtual HRESULT XD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
		                                             D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
		                                             ID3DBlob** errorBlob) = 0;
		virtual HRESULT XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                      const IID& pRootSignatureDeserializerInterface,
		                                                      void** ppRootSignatureDeserializer) = 0;
		virtual HRESULT XD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                               const IID&
		                                                               pRootSignatureDeserializerInterface,
		                                                               void** ppRootSignatureDeserializer) = 0;
		virtual void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                                     PCSTR formatString) = 0;
	};

	class CEDirectXLibHandler : CEDirectXHandler {
	public:
		CEDirectXLibHandler();
		bool Create() override;
		void Destroy() override;
		HRESULT XCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) override;
		HRESULT XDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) override;
		HRESULT XD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
		                           ID3D12Device* device) override;
		HRESULT XD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) override;
		HRESULT XD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
		                                     D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
		                                     ID3DBlob** errorBlob) override;
		HRESULT XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                              const IID& pRootSignatureDeserializerInterface,
		                                              void** ppRootSignatureDeserializer) override;
		HRESULT XD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                       const IID&
		                                                       pRootSignatureDeserializerInterface,
		                                                       void** ppRootSignatureDeserializer) override;
		void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                             PCSTR formatString) override;

		static bool LibCreate();

		static void LibDestroy();

		static HRESULT LibCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory);
		static HRESULT LibDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug);
		static HRESULT LibD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
		                                    ID3D12Device* device);
		static HRESULT LibD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug);
		static HRESULT LibD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
		                                              D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
		                                              ID3DBlob** errorBlob);
		static HRESULT LibD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                       const IID& pRootSignatureDeserializerInterface,
		                                                       void** ppRootSignatureDeserializer);
		static HRESULT LibD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                                const IID&
		                                                                pRootSignatureDeserializerInterface,
		                                                                void** ppRootSignatureDeserializer);
		static void LibSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                                      PCSTR formatString);

	protected:
	private:
	};

	class CEDirectXDLLHandler : CEDirectXHandler {
	public:
		CEDirectXDLLHandler(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib);
		bool Create() override;
		void Destroy() override;
		HRESULT XCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory) override;
		HRESULT XDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug) override;
		HRESULT XD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
		                           ID3D12Device* device) override;
		HRESULT XD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug) override;
		HRESULT XD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
		                                     D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
		                                     ID3DBlob** errorBlob) override;
		HRESULT XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                              const IID& pRootSignatureDeserializerInterface,
		                                              void** ppRootSignatureDeserializer) override;
		HRESULT XD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                       const IID&
		                                                       pRootSignatureDeserializerInterface,
		                                                       void** ppRootSignatureDeserializer) override;
		void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                             PCSTR formatString) override;

		static bool DLLCreate(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib);

		static void DLLDestroy();
		
		static HRESULT DLLCreateDXGIFactory2(UINT Flags, IDXGIFactory2* ppFactory);
		static HRESULT DLLDXGIGetDebugInterface1(UINT Flags, IDXGIDebug1* dxgiDebug);
		static HRESULT DLLD3D12CreateDevice(IDXGIAdapter1* adapter, D3D_FEATURE_LEVEL featureLevel,
		                                    ID3D12Device* device);
		static HRESULT DLLD3D12GetDebugInterface(IDXGIDebug1* dxgiDebug);
		static HRESULT DLLD3D12SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
		                                              D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob,
		                                              ID3DBlob** errorBlob);
		static HRESULT DLLD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                       const IID& pRootSignatureDeserializerInterface,
		                                                       void** ppRootSignatureDeserializer);
		static HRESULT DLLD3D12CreateVersionedRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                                                const IID&
		                                                                pRootSignatureDeserializerInterface,
		                                                                void** ppRootSignatureDeserializer);
		static void DLLSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                                      PCSTR formatString);
	protected:
	private:
		HMODULE DXGILib = 0;
		HMODULE D3D12Lib = 0;
		HMODULE PIXLib = 0;
	};
}

extern ConceptEngine::Graphics::DirectX12::Base::CEDirectXHandler* DXHandler;
