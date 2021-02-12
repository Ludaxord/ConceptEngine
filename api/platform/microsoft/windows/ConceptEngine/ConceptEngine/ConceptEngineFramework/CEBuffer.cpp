#include "CEBuffer.h"

using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEBuffer::CEBuffer(CEDevice& device, const D3D12_RESOURCE_DESC& resDesc) : CEResource(device, resDesc) {
}

CEBuffer::CEBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource) : CEResource(device, resource) {
}
