#include "CEConstantBuffer.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEConstantBuffer::CEConstantBuffer(CEDevice& device,
                                   wrl::ComPtr<ID3D12Resource> resource) : CEBuffer(device, resource) {
	m_sizeInBytes = GetD3D12ResourceDesc().Width;
}

CEConstantBuffer::~CEConstantBuffer() {
}
