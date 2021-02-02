#include "CEAccelerationStructuredBuffer.h"

using namespace Concept::GraphicsEngine::Direct3D;

CEAccelerationStructuredBuffer::CEAccelerationStructuredBuffer(CEDevice& device,
                                                               const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC&
                                                               structDesc) {
}

CEAccelerationStructuredBuffer::CEAccelerationStructuredBuffer(CEDevice& device,
                                                               wrl::ComPtr<ID3D12Resource> scratchResource,
                                                               wrl::ComPtr<ID3D12Resource>
                                                               bottomLevelAccelerationStructuredResource) {
}

void CEAccelerationStructuredBuffer::CreateAccelerationStructuredBufferView() {
}
