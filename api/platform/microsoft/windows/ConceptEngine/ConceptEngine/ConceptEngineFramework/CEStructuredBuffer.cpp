#include "CEStructuredBuffer.h"


#include "CEDevice.h"
#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEStructuredBuffer::CEStructuredBuffer(CEDevice& device, size_t numElements, size_t elementSize)
	: CEBuffer(device,
	           CD3DX12_RESOURCE_DESC::Buffer(numElements * elementSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
	  , m_numElements(numElements)
	  , m_elementSize(elementSize) {
	m_counterBuffer = m_device.CreateByteAddressBuffer(4);
}

CEStructuredBuffer::CEStructuredBuffer(CEDevice& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                                       size_t numElements,
                                       size_t elementSize)
	: CEBuffer(device, resource)
	  , m_numElements(numElements)
	  , m_elementSize(elementSize) {
	m_counterBuffer = m_device.CreateByteAddressBuffer(4);
}
