#include "CEByteAddressBuffer.h"
using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEByteAddressBuffer::CEByteAddressBuffer(CEDevice& device, const D3D12_RESOURCE_DESC& resDesc): CEBuffer(
	device, resDesc) {
}

CEByteAddressBuffer::CEByteAddressBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource): CEBuffer(
	device, resource) {
}
