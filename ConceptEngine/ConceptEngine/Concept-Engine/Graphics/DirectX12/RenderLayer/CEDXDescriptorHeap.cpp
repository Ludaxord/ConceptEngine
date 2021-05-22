#include "CEDXDescriptorHeap.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXDescriptorHeap::CEDXDescriptorHeap(CEDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors,
                                       D3D12_DESCRIPTOR_HEAP_FLAGS flags): CEDXDeviceElement(device), Heap(nullptr),
                                                                           CpuStart({0}), GpuStart({0}),
                                                                           DescriptorHandleIncrementSize(0), Type(type),
                                                                           NumDescriptors(numDescriptors),
                                                                           Flags(flags) {
}

bool CEDXDescriptorHeap::Create() {
}

CEDXOfflineDescriptorHeap::CEDXOfflineDescriptorHeap(CEDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type):
	CEDXDeviceElement(device), Heaps(), Name(), Type(type) {
}

bool CEDXOfflineDescriptorHeap::Create() {
}

D3D12_CPU_DESCRIPTOR_HANDLE CEDXOfflineDescriptorHeap::Allocate(uint32& heapIndex) {
}

void CEDXOfflineDescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32 heapIndex) {
}

void CEDXOfflineDescriptorHeap::SetName(const std::string& name) {
}

bool CEDXOfflineDescriptorHeap::AllocateHeap() {
}

CEDXOnlineDescriptorHeap::CEDXOnlineDescriptorHeap(CEDXDevice* device, uint32 descriptorCount,
                                                   D3D12_DESCRIPTOR_HEAP_TYPE type) : CEDXDeviceElement(device),
	Heap(nullptr), DescriptorCount(descriptorCount), Type(type) {
}

bool CEDXOnlineDescriptorHeap::Create() {
}

uint32 CEDXOnlineDescriptorHeap::AllocateHandles(uint32 numHandles) {
}

bool CEDXOnlineDescriptorHeap::AllocateFreshHeap() {
}

bool CEDXOnlineDescriptorHeap::HasSpace(uint32 numHandles) const {
}

void CEDXOnlineDescriptorHeap::Reset() {
}
