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
#include "CEStructuredBuffer.h"
#include "CESwapChain.h"
#include "CETexture.h"
#include "CEUnorderedAccessView.h"
#include "CEVertexBuffer.h"
using namespace ConceptEngine::GraphicsEngine::Direct3D;

#pragma region Adapters for shared pointers
class CEDeviceInstance : public CEDevice {
public:
	CEDeviceInstance(std::shared_ptr<CEAdapter> adapter): CEDevice(adapter) {

	}

	virtual ~CEDeviceInstance() {
	};
};

class CECommandQueueInstance : public CECommandQueue {
public:
	CECommandQueueInstance(CEDevice& device, D3D12_COMMAND_LIST_TYPE type): CECommandQueue(device, type) {

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

	CEByteAddressBufferInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource)
		: CEByteAddressBuffer(device, resource) {
	}

	virtual ~CEByteAddressBufferInstance() {

	}
};

class CEConstantBufferInstance : public CEConstantBuffer {

public:
	CEConstantBufferInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource)
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

	CEIndexBufferInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT indexFormat)
		: CEIndexBuffer(device, resource, indexFormat) {
	}

	virtual ~CEIndexBufferInstance() {

	}
};

class CEVertexBufferInstance : public CEVertexBuffer {

public:
	CEVertexBufferInstance(CEDevice& device, size_t numVertices, size_t vertexStride)
		: CEVertexBuffer(device, numVertices, vertexStride) {
	}

	CEVertexBufferInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource, size_t numVertices,
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

	CEStructuredBufferInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource, size_t numElements,
	                           size_t elementSize)
		: CEStructuredBuffer(device, resource, numElements, elementSize) {
	}

	virtual ~CEStructuredBufferInstance() {

	}
};

class CETextureInstance : public CETexture {

public:
	CETextureInstance(CEDevice& device, const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE* clearValue)
		: CETexture(device, resourceDesc, clearValue) {
	}

	CETextureInstance(CEDevice& device, const wrl::ComPtr<ID3D12Resource>& resource,
	                  const D3D12_CLEAR_VALUE* clearValue)
		: CETexture(device, resource, clearValue) {
	}

	virtual ~CETextureInstance() {

	}
};

class CERootSignatureInstance : public CERootSignature {

public:
	CERootSignatureInstance(CERootSignature& device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
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
	wrl::ComPtr<ID3D12Debug> debugInterface;
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

std::wstring CEDevice::GetDescription() const {
	return m_adapter->GetDescription();
}

CEDescriptorAllocation CEDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
	return m_descriptorAllocators[type]->Allocate();
}

std::shared_ptr<CESwapChain> CEDevice::CreateSwapChain(HWND hWnd, DXGI_FORMAT backBufferFormat) {
	std::shared_ptr<CESwapChain> swapChain;
	swapChain = std::make_shared<CESwapChainInstance>(*this, hWnd, backBufferFormat);
	return swapChain;
}

std::shared_ptr<CEGUI> CEDevice::CreateGUI(HWND hWnd, const CERenderTarget& renderTarget) {
	std::shared_ptr<CEGUI> gui;
	gui = std::make_shared<CEGUIInstance>(*this, hWnd, renderTarget);
	return gui;
}

std::shared_ptr<CEConstantBuffer> CEDevice::CreateConstantBuffer(wrl::ComPtr<ID3D12Resource> resource) {
}

std::shared_ptr<CEByteAddressBuffer> CEDevice::CreateByteAddressBuffer(size_t bufferSize) {
}

std::shared_ptr<CEByteAddressBuffer> CEDevice::CreateByteAddressBuffer(wrl::ComPtr<ID3D12Resource> resource) {
}

std::shared_ptr<CEStructuredBuffer> CEDevice::CreateStructuredBuffer(size_t numElements, size_t elementSize) {
}

std::shared_ptr<CEStructuredBuffer> CEDevice::CreateStructuredBuffer(wrl::ComPtr<ID3D12Resource> resource,
                                                                     size_t numElements, size_t elementSize) {
}

std::shared_ptr<CETexture> CEDevice::CreateTexture(const D3D12_RESOURCE_DESC& resourceDesc,
                                                   const D3D12_CLEAR_VALUE* clearValue) {
}

std::shared_ptr<CETexture> CEDevice::CreateTexture(wrl::ComPtr<ID3D12Resource> resource,
                                                   const D3D12_CLEAR_VALUE* clearValue) {
}

std::shared_ptr<CEIndexBuffer> CEDevice::CreateIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat) {
}

std::shared_ptr<CEIndexBuffer> CEDevice::CreateIndexBuffer(wrl::ComPtr<ID3D12Resource> resource, size_t numIndicies,
                                                           DXGI_FORMAT indexFormat) {
}

std::shared_ptr<CEVertexBuffer> CEDevice::CreateVertexBuffer(size_t numVertices, size_t vertexStride) {
}

std::shared_ptr<CEVertexBuffer> CEDevice::CreateVertexBuffer(wrl::ComPtr<ID3D12Resource> resource, size_t numVertices,
                                                             size_t vertexStride) {
}

std::shared_ptr<CERootSignature> CEDevice::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignature) {
}

std::shared_ptr<CEConstantBufferView> CEDevice::CreateConstantBufferView(
	const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset) {
}

std::shared_ptr<CEShaderResourceView> CEDevice::CreateShaderResourceView(const std::shared_ptr<CEResource>& resource,
                                                                         const D3D12_SHADER_RESOURCE_VIEW_DESC* srv) {
}

std::shared_ptr<CEUnorderedAccessView> CEDevice::CreateUnorderedAccessView(const std::shared_ptr<CEResource>& resource,
                                                                           const std::shared_ptr<CEResource>
                                                                           counterResource,
                                                                           const D3D12_UNORDERED_ACCESS_VIEW_DESC*
                                                                           uav) {
}

void CEDevice::Flush() {
}

void CEDevice::ReleaseStaleDescriptors() {
}

CECommandQueue& CEDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
}

DXGI_SAMPLE_DESC CEDevice::GetMultiSampleQualityLevels(DXGI_FORMAT format, UINT numSamples,
                                                       D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) const {
}

CEDevice::CEDevice(std::shared_ptr<CEAdapter> adapter): m_adapter(adapter) {
	if (!m_adapter) {
		m_adapter = CEAdapter::Create();
		assert(m_adapter);
	}
	auto dxgiAdapter = m_adapter->GetDXGIAdapter();

	ThrowIfFailed(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

	/**
	 * Enable debug messages (only works if debug layer has already been enabled)
	 */
	wrl::ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(m_device.As(&pInfoQueue))) {
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		/**
		 * Suppress whole categories of messages
		 */
		//D3D12_MESSAGE_CATEGORY Categories[] = {}

		/**
		 * Suppress messages based on their severity level
		 */
		D3D12_MESSAGE_SEVERITY Severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

		/**
		 * Suppress individual messages by their ID
		 */
		D3D12_MESSAGE_ID DenyIDs[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumSeverities = _countof(Severities);
		filter.DenyList.pSeverityList = Severities;
		filter.DenyList.NumIDs = _countof(DenyIDs);
		filter.DenyList.pIDList = DenyIDs;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&filter));
	}

	m_directCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_computeCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	m_copyCommandQueue = std::make_unique<CECommandQueueInstance>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

	/**
	 * Create descriptor allocators
	 */
	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_descriptorAllocators[i] = std::make_unique<CEDescriptorAllocatorInstance>(
			*this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}

	/**
	 * Check features
	 */

	/**
	 * Check highest version of Root Signature
	 */
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData;
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(
			m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(
				D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
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
		m_highestRayTracingTier = featureData.RaytracingTier;
	}
}

CEDevice::~CEDevice() {
}

std::shared_ptr<CEPipelineStateObject> CEDevice::MakePipelineStateObject(
	const D3D12_PIPELINE_STATE_STREAM_DESC& pipelineStateStreamDesc) {
}
