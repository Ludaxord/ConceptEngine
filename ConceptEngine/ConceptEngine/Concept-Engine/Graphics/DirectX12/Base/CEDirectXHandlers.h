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

	enum class CreateOption {
		Lib,
		DLL
	};

	class CEDirectXHandler {
	public:
		CEDirectXHandler() = default;
		virtual ~CEDirectXHandler() = default;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;
		virtual HRESULT XCreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) = 0;
		virtual HRESULT XDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) = 0;
		virtual HRESULT XD3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
		                                   D3D_FEATURE_LEVEL MinimumFeatureLevel,
		                                   _In_ REFIID riid, // Expected: ID3D12Device
		                                   _COM_Outptr_opt_ void** ppDevice) = 0;
		virtual HRESULT XD3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) = 0;
		virtual HRESULT XD3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
		                                             _In_ D3D_ROOT_SIGNATURE_VERSION Version,
		                                             _Out_ ID3DBlob** ppBlob,
		                                             _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob) =
		0;
		virtual HRESULT XD3D12CreateRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer) = 0;
		virtual HRESULT XD3D12CreateVersionedRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer) = 0;
		virtual void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                                     PCSTR formatString) = 0;

		static bool CECreate(CreateOption option, HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib);

		static void CEDestroy();

		static HRESULT CECreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
		static HRESULT CEDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug);
		static HRESULT CED3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
		                                   D3D_FEATURE_LEVEL MinimumFeatureLevel,
		                                   _In_ REFIID riid, // Expected: ID3D12Device
		                                   _COM_Outptr_opt_ void** ppDevice);
		static HRESULT CED3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug);
		static HRESULT CED3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
		                                             _In_ D3D_ROOT_SIGNATURE_VERSION Version,
		                                             _Out_ ID3DBlob** ppBlob,
		                                             _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob);
		static HRESULT CED3D12CreateRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer);
		static HRESULT CED3D12CreateVersionedRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer);
		static void CESetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                                     PCSTR formatString);
	};

	class CEDirectXLibHandler : CEDirectXHandler {
	public:
		CEDirectXLibHandler();
		bool Create() override;
		void Destroy() override;
		HRESULT XCreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) override;
		HRESULT XDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) override;
		HRESULT XD3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
		                           D3D_FEATURE_LEVEL MinimumFeatureLevel,
		                           _In_ REFIID riid, // Expected: ID3D12Device
		                           _COM_Outptr_opt_ void** ppDevice) override;
		HRESULT XD3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) override;
		HRESULT XD3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
		                                     _In_ D3D_ROOT_SIGNATURE_VERSION Version,
		                                     _Out_ ID3DBlob** ppBlob,
		                                     _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob) override;
		HRESULT XD3D12CreateRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer) override;
		HRESULT XD3D12CreateVersionedRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer) override;
		void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                             PCSTR formatString) override;

		static bool LibCreate();

		static void LibDestroy();

	protected:
	private:
	};

	class CEDirectXDLLHandler : CEDirectXHandler {
	public:
		CEDirectXDLLHandler(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib);
		bool Create() override;
		void Destroy() override;
		HRESULT XCreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory) override;
		HRESULT XDXGIGetDebugInterface1(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug) override;
		HRESULT XD3D12CreateDevice(_In_opt_ IUnknown* pAdapter,
		                           D3D_FEATURE_LEVEL MinimumFeatureLevel,
		                           _In_ REFIID riid, // Expected: ID3D12Device
		                           _COM_Outptr_opt_ void** ppDevice) override;
		HRESULT XD3D12GetDebugInterface(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug) override;
		HRESULT XD3D12SerializeRootSignature(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature,
		                                     _In_ D3D_ROOT_SIGNATURE_VERSION Version,
		                                     _Out_ ID3DBlob** ppBlob,
		                                     _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob) override;
		HRESULT XD3D12CreateRootSignatureDeserializer(LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes,
		                                              const IID& pRootSignatureDeserializerInterface,
		                                              void** ppRootSignatureDeserializer) override;
		HRESULT XD3D12CreateVersionedRootSignatureDeserializer(
			_In_reads_bytes_(SrcDataSizeInBytes) LPCVOID pSrcData,
			_In_ SIZE_T SrcDataSizeInBytes,
			_In_ REFIID pRootSignatureDeserializerInterface,
			_Out_ void** ppRootSignatureDeserializer) override;
		void XSetMarkerOnCommandList(ID3D12GraphicsCommandList* commandList, UINT64 color,
		                             PCSTR formatString) override;

		static bool DLLCreate(HMODULE DXGILib, HMODULE D3D12Lib, HMODULE PIXLib);

		static void DLLDestroy();
	protected:
	private:
		HMODULE DXGILib = 0;
		HMODULE D3D12Lib = 0;
		HMODULE PIXLib = 0;
	};
}

extern ConceptEngine::Graphics::DirectX12::Base::CEDirectXHandler* DXHandler;
