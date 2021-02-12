#pragma once
#include <d3d12.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	class CEPipelineConfig;
	class CEShaderConfig;
	class CEExportAssociation;
	class CEHitGroup;
	class CEDXIlLibrary;
	class CEStateObject;
	namespace wrl = Microsoft::WRL;

	class CEGUI;
	class CEDescriptorAllocator;
	class CECommandQueue;
	class CEUnorderedAccessView;
	class CEResource;
	class CECommandList;
	class CEShaderResourceView;
	class CEConstantBufferView;
	class CEPipelineStateObject;
	class CERootSignature;
	class CEVertexBuffer;
	class CEIndexBuffer;
	class CETexture;
	class CEStructuredBuffer;
	class CEByteAddressBuffer;
	class CEConstantBuffer;
	class CERenderTarget;
	class CESwapChain;
	class CEAdapter;
	class CEScene;

	class CEDevice {
	public:
		/**
		 * Debug Layers are abe to catch possible error while creating or updating scene and display it in console.
		 */
		static void EnableDebugLayer();
		static void ReportLiveObjects();

		/**
		 * Create DirectX 12 Device using DirectX Adapter class,
		 * if adapter parameter is nullptr then Device will be created base on highest performance adapter.
		 */
		static std::shared_ptr<CEDevice> Create(std::shared_ptr<CEAdapter> adapter = nullptr);

		/**
		 * Get adapter description that Device is using.
		 */
		std::wstring GetWDescription() const;
		/**
		 * Get adapter description that Device is using as a std::string.
		 */
		std::string GetDescription() const;

		/**
		 * Allocate a number of CPU visible descriptors.
		 */
		CEDescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type,
		                                           uint32_t numDescriptors = 1);

		/**
		 * Get size of handle increment for given type of descriptor heap;
		 */
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const {
			return m_device->GetDescriptorHandleIncrementSize(type);
		}

		/**
		 * Create Swap Chain using the provided OS window handle;
		 * TODO: Change backBufferFormat to capable to use RayTracing, Candidate: DXGI_FORMAT_B8G8R8A8_UNORM
		 */
		std::shared_ptr<CESwapChain> CreateSwapChain(
			HWND hWnd, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM);

		/**
		 * Create GUI object;
		 */
		std::shared_ptr<CEGUI> CreateGUI(HWND hWnd, const CERenderTarget& renderTarget);

		/**
		 * Create Constant Buffer from passed resource;
		 */
		std::shared_ptr<CEConstantBuffer> CreateConstantBuffer(wrl::ComPtr<ID3D12Resource> resource);

		/**
		 * Create Byte Address Buffer resource
		 * @param resDesc = description of resource;
		 */
		std::shared_ptr<CEByteAddressBuffer> CreateByteAddressBuffer(size_t bufferSize);
		std::shared_ptr<CEByteAddressBuffer> CreateByteAddressBuffer(
			wrl::ComPtr<ID3D12Resource> resource);

		/**
		 * Create Structured buffer resource;
		 */
		std::shared_ptr<CEStructuredBuffer> CreateStructuredBuffer(
			size_t numElements, size_t elementSize);
		std::shared_ptr<CEStructuredBuffer> CreateStructuredBuffer(wrl::ComPtr<ID3D12Resource> resource,
		                                                           size_t numElements, size_t elementSize);

		/**
		 * Create Texture resource;
		 *
		 * @param ResourceDesc = description of texture to create;
		 * @param [ClearValue] Optional optimized clear value for the texture;
		 * @param [TextureUsage] Optional texture usage flag provides a hint about how the texture will be used;
		 */
		std::shared_ptr<CETexture> CreateTexture(const D3D12_RESOURCE_DESC& resourceDesc,
		                                         const D3D12_CLEAR_VALUE* clearValue = nullptr);
		std::shared_ptr<CETexture> CreateTexture(wrl::ComPtr<ID3D12Resource> resource,
		                                         const D3D12_CLEAR_VALUE* clearValue = nullptr);


		/**
		 * Load DXIL Library
		 *
		 * @param shaderFile, path to shader file.
		 * @param entryPoints
		 */
		std::shared_ptr<CEDXIlLibrary> LoadDXILLibrary(std::wstring shaderFile, const WCHAR* entryPoints[]) const;

		/**
		 * Create Hit Group
		 *
		 * @param ahsExport
		 * @param chsExport
		 * @param name
		 */
		std::shared_ptr<CEHitGroup>
		CreateHitGroup(LPCWSTR ahsExport, LPCWSTR chsExport, const std::wstring& name) const;

		/**
		 * Create Export Association
		 *
		 * @param exportNames
		 * @param pSubObjectToAssociate
		 */
		std::shared_ptr<CEExportAssociation> CreateExportAssociation(const WCHAR* exportNames[],
		                                                             const D3D12_STATE_SUBOBJECT* pSubObjectToAssociate)
		const;

		/**
		 * Create Shader Config
		 *
		 * @param maxAttributeSizeInBytes
		 * @param maxPayloadSizeInBytes
		 */
		std::shared_ptr<CEShaderConfig> CreateShaderConfig(uint32_t maxAttributeSizeInBytes,
		                                                             uint32_t maxPayloadSizeInBytes) const;

		/**
		 * Create Pipeline Config
		 *
		 * @param maxAttributeSizeInBytes
		 */
		std::shared_ptr<CEPipelineConfig> CreatePipelineConfig(uint32_t maxTraceRecursionDepth) const;

		/**
		 * Create Index Buffer resource;
		 */
		std::shared_ptr<CEIndexBuffer> CreateIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat);
		std::shared_ptr<CEIndexBuffer> CreateIndexBuffer(wrl::ComPtr<ID3D12Resource> resource,
		                                                 size_t numIndicies,
		                                                 DXGI_FORMAT indexFormat);

		/**
		 * Create Vertex Buffer resource;
		 */
		std::shared_ptr<CEVertexBuffer> CreateVertexBuffer(size_t numVertices, size_t vertexStride);
		std::shared_ptr<CEVertexBuffer> CreateVertexBuffer(
			wrl::ComPtr<ID3D12Resource> resource, size_t numVertices,
			size_t vertexStride);

		/**
		 * Create Root Signature
		 */
		std::shared_ptr<CERootSignature> CreateRootSignature(
			const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

		/**
		 * Create Pipeline State Object based on passed template PipelineStateStream
		 */
		template <class PipelineStateStream>
		std::shared_ptr<CEPipelineStateObject> CreatePipelineStateObject(
			PipelineStateStream& pipelineStateStream) {
			const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				sizeof(PipelineStateStream), &pipelineStateStream
			};
			return MakePipelineStateObject(pipelineStateStreamDesc);
		}

		std::shared_ptr<CEStateObject> CreateStateObject(
			std::vector<D3D12_STATE_SUBOBJECT>& stateObjects,
			D3D12_STATE_OBJECT_TYPE type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) {
			const auto size = stateObjects.size();
			const D3D12_STATE_OBJECT_DESC stateStreamDesc = {
				type, size, stateObjects.data()
			};
			return MakeStateObject(stateStreamDesc);
		}

		/**
		 * Create Constant Buffer View
		 */
		std::shared_ptr<CEConstantBufferView> CreateConstantBufferView(
			const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset = 0);

		/**
		 * Create Shader Resource View
		 */
		std::shared_ptr<CEShaderResourceView> CreateShaderResourceView(
			const std::shared_ptr<CEResource>& resource,
			const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
		/**
		 * Create Unordered Access View
		 */
		std::shared_ptr<CEUnorderedAccessView> CreateUnorderedAccessView(const std::shared_ptr<CEResource>& resource,
		                                                                 const std::shared_ptr<CEResource>&
			                                                                 counterResource = nullptr,
		                                                                 const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav =
			                                                                 nullptr);

		/**
		 * Flush all command Queues;
		 */
		void Flush();

		/**
		 * Release State descriptors, function should be called when frame counter is completed;
		 */
		void ReleaseStaleDescriptors();

		/**
		 * Get adapter that was used to create device;
		 */
		std::shared_ptr<CEAdapter> GetAdapter() const {
			return m_adapter;
		}

		/**
		 * Get Command Queue
		 * Command Queue Types:
		 * - D3D12_COMMAND_LIST_TYPE_DIRECT: Can be used to draw, dispatch or copy commands;
		 * - D3D12_COMMAND_LIST_TYPE_COMPUTE: Can be used to dispatch or copy commands;
		 * - D3D12_COMMAND_LIST_TYPE_COPY: Can be used to copy commands;
		 * Default is D3D12_COMMAND_LIST_TYPE_DIRECT;
		 */
		CECommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		wrl::ComPtr<ID3D12Device5> GetDevice() const {
			return m_device;
		}

		wrl::ComPtr<ID3D12Device5> GetRayTracingDevice() const {
			return m_rtxDevice;
		}

		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const {
			return m_highestRootSignatureVersion;
		}

		D3D12_RAYTRACING_TIER GetRayTracingTier() const {
			return m_rayTracingTier;
		}

		std::wstring GetCurrentRayTracingSupportName() {
			return GetRayTracingSupportName(m_rayTracingTier);
		}

		std::wstring GetRayTracingSupportName(D3D12_RAYTRACING_TIER rtxTier) {
			std::wstring rtxSupportDescription;
			switch (rtxTier) {
			case D3D12_RAYTRACING_TIER_NOT_SUPPORTED:
				rtxSupportDescription = L"Ray Tracing No Support";
				break;
			case D3D12_RAYTRACING_TIER_1_0:
				rtxSupportDescription = L"Ray Tracing Support: 1.0";
				break;
			case D3D12_RAYTRACING_TIER_1_1:
				rtxSupportDescription = L"Ray Tracing Support: 1.1";
				break;
			default:
				rtxSupportDescription = L"Information Unavailable";
				break;
			};
			return rtxSupportDescription;
		}

		DXGI_SAMPLE_DESC GetMultiSampleQualityLevels(DXGI_FORMAT format,
		                                             UINT numSamples = D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT,
		                                             D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags =
			                                             D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;

	protected:
		/**
		* Default Constructors/Destructors
		*/
		explicit CEDevice(std::shared_ptr<CEAdapter> adapter);
		virtual ~CEDevice();

		/*
		 * Ray tracing Device interface
		 */
		void CreateRayTracingDevice();

		/**
		 * Make Pipeline State Object
		 */
		std::shared_ptr<CEPipelineStateObject> MakePipelineStateObject(
			const D3D12_PIPELINE_STATE_STREAM_DESC& pipelineStateStreamDesc);

		/**
		 * Make State Object
		 */
		std::shared_ptr<CEStateObject> MakeStateObject(const D3D12_STATE_OBJECT_DESC& stateStreamDesc);

	private:
		wrl::ComPtr<ID3D12Device5> m_device;
		wrl::ComPtr<ID3D12Device5> m_rtxDevice;

		std::unique_ptr<CECommandQueue> m_directCommandQueue;
		std::unique_ptr<CECommandQueue> m_computeCommandQueue;
		std::unique_ptr<CECommandQueue> m_copyCommandQueue;

		std::unique_ptr<CEDescriptorAllocator> m_descriptorAllocators[
			D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		std::shared_ptr<CEAdapter> m_adapter;
		D3D_ROOT_SIGNATURE_VERSION m_highestRootSignatureVersion;
		D3D12_RAYTRACING_TIER m_rayTracingTier;
	};
}
