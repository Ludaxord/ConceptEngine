#include "CEDevice.h"

#include "CEAdapter.h"
#include "CEByteAddressBuffer.h"
#include "CECommandQueue.h"
#include "CEConstantBuffer.h"
#include "CEConstantBufferView.h"
#include "CEDescriptorAllocator.h"
#include "CEDX12Libs.h"
#include "CEGUI.h"

#include "CEHelper.h"
#include "CEIndexBuffer.h"
#include "CEPipelineStateObject.h"
#include "CERootSignature.h"
#include "CEShaderResourceView.h"
#include "CEStateObject.h"
#include "CEStructuredBuffer.h"
#include "CESwapChain.h"
#include "CETexture.h"
#include "CEUnorderedAccessView.h"
#include "CEVertexBuffer.h"
using namespace Concept::GraphicsEngine::Direct3D;

#pragma region Instances for shared pointers
class CEDeviceInstance : public CEDevice {
public:
	CEDeviceInstance(std::shared_ptr<CEAdapter> adapter) : CEDevice(adapter) {

	}

	virtual ~CEDeviceInstance() {
	};
};

class CECommandQueueInstance : public CECommandQueue {
public:
	CECommandQueueInstance(CEDevice& device, D3D12_COMMAND_LIST_TYPE type) : CECommandQueue(device, type) {

	}

	virtual ~CECommandQueueInstance() {

	}
};

class CEDescriptorAllocatorInstance : public CEDescriptorAllocator {

public:
	CEDescriptorAllocatorInstance(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
	                              uint32_t numDescriptorsPerHeap = 256)
		: CEDescriptorAllocator(device, type, numDescriptorsPerHeap) {
	}

	virtual ~CEDescriptorAllocatorInstance() {

	}
};

class CESwapChainInstance : public CESwapChain {

public:
	CESwapChainInstance(CEDevice& device, HWND hWnd, DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R10G10B10A2_UNORM)
		: CESwapChain(device, hWnd, renderTargetFormat) {
	}

	virtual ~CESwapChainInstance() {
	}
};

class CEGUIInstance : public CEGUI {

public:
	CEGUIInstance(CEDevice& device, HWND hWnd, const CERenderTarget& renderTarget)
		: CEGUI(device, hWnd, renderTarget) {
	}

	virtual ~CEGUIInstance() {

	}
};

class CEByteAddressBufferInstance : public CEByteAddressBuffer {

public:
	CEByteAddressBufferInstance(CEDevice& device, const D3D12_RESOURCE_DESC& resDesc)
		: CEByteAddressBuffer(device, resDesc) {
	}

	CEByteAddressBufferInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource)
		: CEByteAddressBuffer(device, resource) {
	}

	virtual ~CEByteAddressBufferInstance() {

	}
};

class CEConstantBufferInstance : public CEConstantBuffer {

public:
	CEConstantBufferInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource)
		: CEConstantBuffer(device, resource) {
	}

	virtual ~CEConstantBufferInstance() {

	}
};

class CEIndexBufferInstance : public CEIndexBuffer {

public:
	CEIndexBufferInstance(CEDevice& device, size_t numIndices, DXGI_FORMAT indexFormat)
		: CEIndexBuffer(device, numIndices, indexFormat) {
	}

	CEIndexBufferInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource, size_t numIndices,
	                      DXGI_FORMAT indexFormat)
		: CEIndexBuffer(device, resource, numIndices, indexFormat) {
	}

	virtual ~CEIndexBufferInstance() {

	}
};

class CEVertexBufferInstance : public CEVertexBuffer {

public:
	CEVertexBufferInstance(CEDevice& device, size_t numVertices, size_t vertexStride)
		: CEVertexBuffer(device, numVertices, vertexStride) {
	}

	CEVertexBufferInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource, size_t numVertices,
	                       size_t vertexStride)
		: CEVertexBuffer(device, resource, numVertices, vertexStride) {
	}

	virtual ~CEVertexBufferInstance() {

	}
};

class CEStructuredBufferInstance : public CEStructuredBuffer {
public:
	CEStructuredBufferInstance(CEDevice& device, size_t numElements, size_t elementSize)
		: CEStructuredBuffer(device, numElements, elementSize) {
	}

	CEStructuredBufferInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource, size_t numElements,
	                           size_t elementSize)
		: CEStructuredBuffer(device, resource, numElements, elementSize) {
	}

	virtual ~CEStructuredBufferInstance() {

	}
};

class CETextureInstance : public CETexture {

public:
	CETextureInstance(CEDevice& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
		: CETexture(device, resourceDesc, clearValue) {
	}

	CETextureInstance(CEDevice& device, const ComPtr<ID3D12Resource>& resource,
	                  const D3D12_CLEAR_VALUE* clearValue)
		: CETexture(device, resource, clearValue) {
	}

	virtual ~CETextureInstance() {

	}
};

class CERootSignatureInstance : public CERootSignature {

public:
	CERootSignatureInstance(CEDevice& device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
		: CERootSignature(device, rootSignatureDesc) {
	}

	virtual ~CERootSignatureInstance() {

	}
};

class CEPipelineStateObjectInstance : public CEPipelineStateObject {

public:
	CEPipelineStateObjectInstance(CEDevice& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc)
		: CEPipelineStateObject(device, desc) {
	}

	virtual ~CEPipelineStateObjectInstance() {

	}
};

class CEStateObjectInstance : public CEStateObject {

public:
	CEStateObjectInstance(CEDevice& device, const D3D12_STATE_OBJECT_DESC& desc): CEStateObject(device, desc) {
	}

	virtual ~CEStateObjectInstance() {

	}
};

class CEConstantBufferViewInstance : public CEConstantBufferView {

public:
	CEConstantBufferViewInstance(CEDevice& device, const std::shared_ptr<CEConstantBuffer>& constantBuffer,
	                             size_t offset)
		: CEConstantBufferView(device, constantBuffer, offset) {
	}

	virtual ~CEConstantBufferViewInstance() {

	}
};

class CEShaderResourceViewInstance : public CEShaderResourceView {

public:
	CEShaderResourceViewInstance(CEDevice& device, const std::shared_ptr<CEResource>& resource,
	                             const D3D12_SHADER_RESOURCE_VIEW_DESC* srv)
		: CEShaderResourceView(device, resource, srv) {
	}

	virtual ~CEShaderResourceViewInstance() {

	}
};

class CEUnorderedAccessViewInstance : public CEUnorderedAccessView {

public:
	CEUnorderedAccessViewInstance(CEDevice& device, const std::shared_ptr<CEResource>& resource,
	                              const std::shared_ptr<CEResource>& counterResource,
	                              const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav)
		: CEUnorderedAccessView(device, resource, counterResource, uav) {
	}

	virtual ~CEUnorderedAccessViewInstance() {

	}
};
#pragma endregion

void CEDevice::EnableDebugLayer() {
	ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
}

void CEDevice::ReportLiveObjects() {

	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
}

std::shared_ptr<CEDevice> CEDevice::Create(std::shared_ptr<CEAdapter> adapter) {
	return std::make_shared<CEDeviceInstance>(adapter);
}

std::string CEDevice::GetDescription() const {
	auto wDesc = GetWDescription();
	std::string sDesc(wDesc.begin(), wDesc.end());
	return sDesc;
}

std::wstring CEDevice::GetWDescription() const {
	return m_adapter->GetDescription();
}


CEDevice::CEDevice(std::shared_ptr<CEAdapter> adapter): m_adapter(adapter) {
	if (!m_adapter) {
		m_adapter = CEAdapter::Create();
		assert(m_adapter);
	}

	auto dxgiAdapter = m_adapter->GetDXGIAdapter();

	ThrowIfFailed(D3D12CreateDevice(dxgiAdapter.Get(), m_adapter->GetFeatureLevel(), IID_PPV_ARGS(&m_device)));

	spdlog::info("Device with Feature Level: {} Created", m_adapter->GetFeatureLevelName());

	// Enable debug messages (only works if the debug layer has already been enabled).
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(m_device.As(&pInfoQueue))) {
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		// Suppress whole categories of messages
		// D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE, // I'm really not sure how to avoid this
			// message.

			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE, // This warning occurs when using capture frame while graphics
			// debugging.

			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE, // This warning occurs when using capture frame while graphics
			// debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		// NewFilter.DenyList.NumCategories = _countof(Categories);
		// NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}

	m_directCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_computeCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	m_copyCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

	// Create descriptor allocators
	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_descriptorAllocators[i] =
			std::make_unique<CEDescriptorAllocatorInstance>(*this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}

	// Check features.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData;
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
			sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}
		m_highestRootSignatureVersion = featureData.HighestVersion;
	}

	/**
	 * Check Ray Tracing support
	 */
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureData = {};

		if (FAILED(
			m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureData, sizeof(
				D3D12_FEATURE_DATA_D3D12_OPTIONS5)))) {
			featureData.RaytracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		}
		m_rayTracingTier = featureData.RaytracingTier;
	}

	if (GetRayTracingTier() != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
		CreateRayTracingDevice();
	}
}

CEDevice::~CEDevice() {
}

void CEDevice::CreateRayTracingDevice() {
	ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&m_rtxDevice)));
}

CECommandQueue& CEDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
	CECommandQueue* commandQueue = nullptr;
	switch (type) {
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		commandQueue = m_directCommandQueue.get();
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		commandQueue = m_computeCommandQueue.get();
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		commandQueue = m_copyCommandQueue.get();
		break;
	default:
		assert(false && "Invalid command queue type.");
	}

	return *commandQueue;
}

void CEDevice::Flush() {
	m_directCommandQueue->Flush();
	m_computeCommandQueue->Flush();
	m_copyCommandQueue->Flush();
}

CEDescriptorAllocation CEDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
	return m_descriptorAllocators[type]->Allocate(numDescriptors);
}

void CEDevice::ReleaseStaleDescriptors() {
	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_descriptorAllocators[i]->ReleaseStaleDescriptors();
	}
}

std::shared_ptr<CESwapChain> CEDevice::CreateSwapChain(HWND hWnd, DXGI_FORMAT backBufferFormat) {
	std::shared_ptr<CESwapChain> swapChain;
	swapChain = std::make_shared<CESwapChainInstance>(*this, hWnd, backBufferFormat);

	return swapChain;
}

std::shared_ptr<CEGUI> CEDevice::CreateGUI(HWND hWnd, const CERenderTarget& renderTarget) {
	std::shared_ptr<CEGUI> gui = std::make_shared<CEGUIInstance>(*this, hWnd, renderTarget);

	return gui;
}

std::shared_ptr<CEConstantBuffer> CEDevice::CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	std::shared_ptr<CEConstantBuffer> constantBuffer = std::make_shared<CEConstantBufferInstance>(*this, resource);

	return constantBuffer;
}

std::shared_ptr<CEByteAddressBuffer> CEDevice::CreateByteAddressBuffer(size_t bufferSize) {
	// Align-up to 4-bytes
	bufferSize = Math::AlignUp(bufferSize, 4);

	std::shared_ptr<CEByteAddressBuffer> buffer = std::make_shared<CEByteAddressBufferInstance>(
		*this, CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

	return buffer;
}

std::shared_ptr<CEByteAddressBuffer> CEDevice::CreateByteAddressBuffer(ComPtr<ID3D12Resource> resource) {
	std::shared_ptr<CEByteAddressBuffer> buffer = std::make_shared<CEByteAddressBufferInstance>(*this, resource);

	return buffer;
}

std::shared_ptr<CEStructuredBuffer> CEDevice::CreateStructuredBuffer(size_t numElements, size_t elementSize) {
	std::shared_ptr<CEStructuredBuffer> structuredBuffer =
		std::make_shared<CEStructuredBufferInstance>(*this, numElements, elementSize);

	return structuredBuffer;
}

std::shared_ptr<CEStructuredBuffer> CEDevice::CreateStructuredBuffer(ComPtr<ID3D12Resource> resource,
                                                                     size_t numElements,
                                                                     size_t elementSize) {
	std::shared_ptr<CEStructuredBuffer> structuredBuffer =
		std::make_shared<CEStructuredBufferInstance>(*this, resource, numElements, elementSize);

	return structuredBuffer;
}

std::shared_ptr<CEIndexBuffer> CEDevice::CreateIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat) {
	std::shared_ptr<CEIndexBuffer> indexBuffer = std::make_shared<CEIndexBufferInstance>(
		*this, numIndicies, indexFormat);

	return indexBuffer;
}

std::shared_ptr<CEIndexBuffer>
CEDevice::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numIndices,
                            DXGI_FORMAT indexFormat) {
	std::shared_ptr<CEIndexBuffer> indexBuffer =
		std::make_shared<CEIndexBufferInstance>(*this, resource, numIndices, indexFormat);

	return indexBuffer;
}

std::shared_ptr<CEVertexBuffer> CEDevice::CreateVertexBuffer(size_t numVertices, size_t vertexStride) {
	std::shared_ptr<CEVertexBuffer> vertexBuffer = std::make_shared<CEVertexBufferInstance>(
		*this, numVertices, vertexStride);

	return vertexBuffer;
}

std::shared_ptr<CEVertexBuffer>
CEDevice::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numVertices,
                             size_t vertexStride) {
	std::shared_ptr<CEVertexBuffer> vertexBuffer =
		std::make_shared<CEVertexBufferInstance>(*this, resource, numVertices, vertexStride);

	return vertexBuffer;
}

std::shared_ptr<CETexture> CEDevice::CreateTexture(const D3D12_RESOURCE_DESC& resourceDesc,
                                                   const D3D12_CLEAR_VALUE* clearValue) {
	std::shared_ptr<CETexture> texture = std::make_shared<CETextureInstance>(*this, resourceDesc, clearValue);

	return texture;
}

std::shared_ptr<CETexture> CEDevice::CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                                                   const D3D12_CLEAR_VALUE* clearValue) {
	std::shared_ptr<CETexture> texture = std::make_shared<CETextureInstance>(*this, resource, clearValue);

	return texture;
}

std::shared_ptr<CERootSignature>
CEDevice::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc) {
	std::shared_ptr<CERootSignature> rootSignature = std::make_shared<CERootSignatureInstance
	>(*this, rootSignatureDesc);

	return rootSignature;
}

std::shared_ptr<CEPipelineStateObject> CEDevice::MakePipelineStateObject(
	const D3D12_PIPELINE_STATE_STREAM_DESC& pipelineStateStreamDesc) {
	std::shared_ptr<CEPipelineStateObject> pipelineStateObject =
		std::make_shared<CEPipelineStateObjectInstance>(*this, pipelineStateStreamDesc);

	return pipelineStateObject;
}

std::shared_ptr<CEStateObject> CEDevice::MakeStateObject(const D3D12_STATE_OBJECT_DESC& stateStreamDesc) {
	std::shared_ptr<CEStateObject> pipelineStateObject = std::make_shared<CEStateObjectInstance
	>(*this, stateStreamDesc);

	return pipelineStateObject;
}

std::shared_ptr<CEConstantBufferView>
CEDevice::CreateConstantBufferView(const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset) {
	std::shared_ptr<CEConstantBufferView> constantBufferView =
		std::make_shared<CEConstantBufferViewInstance>(*this, constantBuffer, offset);

	return constantBufferView;
}

std::shared_ptr<CEShaderResourceView> CEDevice::CreateShaderResourceView(const std::shared_ptr<CEResource>& resource,
                                                                         const D3D12_SHADER_RESOURCE_VIEW_DESC* srv) {
	std::shared_ptr<CEShaderResourceView> shaderResourceView =
		std::make_shared<CEShaderResourceViewInstance>(*this, resource, srv);

	return shaderResourceView;
}

std::shared_ptr<CEUnorderedAccessView>
CEDevice::CreateUnorderedAccessView(const std::shared_ptr<CEResource>& resource,
                                    const std::shared_ptr<CEResource>& counterResource,
                                    const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav) {
	std::shared_ptr<CEUnorderedAccessView> unorderedAccessView =
		std::make_shared<CEUnorderedAccessViewInstance>(*this, resource, counterResource, uav);

	return unorderedAccessView;
}

DXGI_SAMPLE_DESC CEDevice::GetMultiSampleQualityLevels(DXGI_FORMAT format, UINT numSamples,
                                                       D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) const {
	DXGI_SAMPLE_DESC sampleDesc = {1, 0};

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
	qualityLevels.Format = format;
	qualityLevels.SampleCount = 1;
	qualityLevels.Flags = flags;
	qualityLevels.NumQualityLevels = 0;

	while (
		qualityLevels.SampleCount <= numSamples &&
		SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels,
			sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS))) &&
		qualityLevels.NumQualityLevels > 0) {
		// That works...
		sampleDesc.Count = qualityLevels.SampleCount;
		sampleDesc.Quality = qualityLevels.NumQualityLevels - 1;

		// But can we do better?
		qualityLevels.SampleCount *= 2;
	}

	return sampleDesc;
}
