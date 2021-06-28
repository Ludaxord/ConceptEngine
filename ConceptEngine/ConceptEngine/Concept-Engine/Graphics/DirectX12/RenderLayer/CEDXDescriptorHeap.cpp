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
	D3D12_DESCRIPTOR_HEAP_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.Type = Type;
	desc.Flags = Flags;
	desc.NumDescriptors = NumDescriptors;

	HRESULT hResult = GetDevice()->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&Heap));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXDescriptorHeap]: Failed to create Descriptor Heap");
		return false;
	}

	CE_LOG_INFO("[CEDXDescriptorHeap]: Descriptor Heap Created");

	CpuStart = Heap->GetCPUDescriptorHandleForHeapStart();
	GpuStart = Heap->GetGPUDescriptorHandleForHeapStart();
	DescriptorHandleIncrementSize = GetDevice()->GetDescriptorHandleIncrementSize(desc.Type);

	return true;
}

CEDXOfflineDescriptorHeap::CEDXOfflineDescriptorHeap(CEDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type):
	CEDXDeviceElement(device), Heaps(), Name(), Type(type) {
}

bool CEDXOfflineDescriptorHeap::Create() {
	DescriptorSize = GetDevice()->GetDescriptorHandleIncrementSize(Type);
	return AllocateHeap();
}

D3D12_CPU_DESCRIPTOR_HANDLE CEDXOfflineDescriptorHeap::Allocate(uint32& oHeapIndex) {
	uint32 heapIndex = 0;
	bool foundHeap = false;
	for (CEDescriptorHeap& heap : Heaps) {
		if (!heap.FreeList.IsEmpty()) {
			foundHeap = true;
			break;
		}

		heapIndex++;
	}

	if (!foundHeap) {
		if (!AllocateHeap()) {
			return {0};
		}

		heapIndex = static_cast<uint32>(Heaps.Size()) - 1;
	}

	CEDescriptorHeap& heap = Heaps[heapIndex];
	CEDescriptorRange& range = heap.FreeList.Front();

	D3D12_CPU_DESCRIPTOR_HANDLE handle = range.Begin;
	range.Begin.ptr += DescriptorSize;

	if (!range.IsValid()) {
		heap.FreeList.Erase(heap.FreeList.Begin());
	}

	oHeapIndex = heapIndex;
	return handle;
}

void CEDXOfflineDescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32 heapIndex) {
	Assert(heapIndex < Heaps.Size());
	CEDescriptorHeap& heap = Heaps[heapIndex];

	bool foundRange = false;
	for (CEDescriptorRange& range : heap.FreeList) {
		Assert(range.IsValid());
		if (handle.ptr + DescriptorSize == range.Begin.ptr) {
			range.Begin = handle;
			foundRange = true;
			break;
		}
		else if (handle.ptr == range.End.ptr) {
			range.End.ptr += DescriptorSize;
			foundRange = true;
			break;
		}
	}

	if (!foundRange) {
		D3D12_CPU_DESCRIPTOR_HANDLE end = {handle.ptr + DescriptorSize};
		heap.FreeList.EmplaceBack(handle, end);
	}
}

CEDXOnlineDescriptorHeap::CEDXOnlineDescriptorHeap(CEDXDevice* device, uint32 descriptorCount,
                                                   D3D12_DESCRIPTOR_HEAP_TYPE type): CEDXDeviceElement(device),
	Heap(nullptr), DescriptorCount(descriptorCount), Type(type) {
}

bool CEDXOnlineDescriptorHeap::Create() {
	Heap = new CEDXDescriptorHeap(GetDevice(), Type, DescriptorCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	if (Heap->Create()) {
		return true;
	}

	return false;
}

uint32 CEDXOnlineDescriptorHeap::AllocateHandles(uint32 numHandles) {
	Assert(numHandles <= DescriptorCount);

	if (!HasSpace(numHandles)) {
		if (!AllocateFreshHeap()) {
			return (uint32)-1;
		}
	}

	const uint32 Handle = CurrentHandle;
	CurrentHandle += numHandles;
	return Handle;
}

bool CEDXOnlineDescriptorHeap::AllocateFreshHeap() {
	DiscardedHeaps.EmplaceBack(Heap);

	if (HeapPool.IsEmpty()) {
		Heap = new CEDXDescriptorHeap(GetDevice(), Type, DescriptorCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		if (!Heap->Create()) {
			return false;
		}
	}
	else {
		Heap = HeapPool.Back();
		HeapPool.PopBack();
	}

	CurrentHandle = 0;
	return true;
}

bool CEDXOnlineDescriptorHeap::HasSpace(uint32 numHandles) const {
	const uint32 NewCurrentHandle = CurrentHandle + numHandles;
	return NewCurrentHandle < DescriptorCount;
}

void CEDXOnlineDescriptorHeap::Reset() {
	if (!HeapPool.IsEmpty()) {
		for (CERef<CEDXDescriptorHeap>& CurrentHeap : DiscardedHeaps) {
			HeapPool.EmplaceBack(CurrentHeap);
		}

		DiscardedHeaps.Clear();
	}
	else {
		HeapPool.Swap(DiscardedHeaps);
	}

	CurrentHandle = 0;
}

void CEDXOfflineDescriptorHeap::SetName(const std::string& name) {
	Name = name;
	uint32 heapIndex = 0;
	for (CEDescriptorHeap& heap : Heaps) {
		std::string dhName = Name + "[" + std::to_string(heapIndex) + "]";
		heap.Heap->SetName(dhName.c_str());
	}
}

bool CEDXOfflineDescriptorHeap::AllocateHeap() {
	constexpr uint32 descriptorCount = D3D12_MAX_OFFLINE_DESCRIPTOR_COUNT;

	CERef<CEDXDescriptorHeap> heap = new CEDXDescriptorHeap(
		GetDevice(), Type, descriptorCount, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	if (heap->Create()) {
		if (!Name.empty()) {
			std::string dhName = Name + std::to_string(Heaps.Size());
			heap->SetName(dhName.c_str());
		}

		Heaps.EmplaceBack(heap);
		return true;
	}

	return false;
}
