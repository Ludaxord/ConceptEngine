#include "CEDescriptorAllocatorPage.h"

#include "CEDevice.h"
#include "CEHelper.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEDescriptorAllocatorPage::CEDescriptorAllocatorPage(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                     uint32_t numDescriptors)
	: m_device(device)
	  , m_heapType(type)
	  , m_numDescriptorsInHeap(numDescriptors) {
	auto d3d12Device = m_device.GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = m_heapType;
	heapDesc.NumDescriptors = m_numDescriptorsInHeap;

	ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)));

	m_baseDescriptor = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_descriptorHandleIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(m_heapType);
	m_numFreeHandles = m_numDescriptorsInHeap;

	// Initialize the free lists
	AddNewBlock(0, m_numFreeHandles);
}

D3D12_DESCRIPTOR_HEAP_TYPE CEDescriptorAllocatorPage::GetHeapType() const {
	return m_heapType;
}

uint32_t CEDescriptorAllocatorPage::NumFreeHandles() const {
	return m_numFreeHandles;
}

bool CEDescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const {
	return m_freeListBySize.lower_bound(numDescriptors) != m_freeListBySize.end();
}

void CEDescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors) {
	auto offsetIt = m_freeListByOffset.emplace(offset, numDescriptors);
	auto sizeIt = m_freeListBySize.emplace(numDescriptors, offsetIt.first);
	offsetIt.first->second.FreeListBySizeIt = sizeIt;
}

CEDescriptorAllocation CEDescriptorAllocatorPage::Allocate(uint32_t numDescriptors) {
	std::lock_guard<std::mutex> lock(m_allocationMutex);

	// There are less than the requested number of descriptors left in the heap.
	// Return a NULL descriptor and try another heap.
	if (numDescriptors > m_numFreeHandles) {
		return CEDescriptorAllocation();
	}

	// Get the first block that is large enough to satisfy the request.
	auto smallestBlockIt = m_freeListBySize.lower_bound(numDescriptors);
	if (smallestBlockIt == m_freeListBySize.end()) {
		// There was no free block that could satisfy the request.
		return CEDescriptorAllocation();
	}

	// The size of the smallest block that satisfies the request.
	auto blockSize = smallestBlockIt->first;

	// The pointer to the same entry in the FreeListByOffset map.
	auto offsetIt = smallestBlockIt->second;

	// The offset in the descriptor heap.
	auto offset = offsetIt->first;

	// Remove the existing free block from the free list.
	m_freeListBySize.erase(smallestBlockIt);
	m_freeListByOffset.erase(offsetIt);

	// Compute the new free block that results from splitting this block.
	auto newOffset = offset + numDescriptors;
	auto newSize = blockSize - numDescriptors;

	if (newSize > 0) {
		// If the allocation didn't exactly match the requested size,
		// return the left-over to the free list.
		AddNewBlock(newOffset, newSize);
	}

	// Decrement free handles.
	m_numFreeHandles -= numDescriptors;

	return CEDescriptorAllocation(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_baseDescriptor, offset, m_descriptorHandleIncrementSize), numDescriptors,
		m_descriptorHandleIncrementSize, shared_from_this());
}

uint32_t CEDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	return static_cast<uint32_t>(handle.ptr - m_baseDescriptor.ptr) / m_descriptorHandleIncrementSize;
}

void CEDescriptorAllocatorPage::Free(CEDescriptorAllocation&& descriptor) {
	// Compute the offset of the descriptor within the descriptor heap.
	auto offset = ComputeOffset(descriptor.GetDescriptorHandle());

	std::lock_guard<std::mutex> lock(m_allocationMutex);
	// Don't add the block directly to the free list until the frame has completed.
	m_staleDescriptors.emplace(offset, descriptor.GetNumHandles());
}

void CEDescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors) {
	// Find the first element whose offset is greater than the specified offset.
	// This is the block that should appear after the block that is being freed.
	auto nextBlockIt = m_freeListByOffset.upper_bound(offset);

	// Find the block that appears before the block being freed.
	auto prevBlockIt = nextBlockIt;
	// If it's not the first block in the list.
	if (prevBlockIt != m_freeListByOffset.begin()) {
		// Go to the previous block in the list.
		--prevBlockIt;
	}
	else {
		// Otherwise, just set it to the end of the list to indicate that no
		// block comes before the one being freed.
		prevBlockIt = m_freeListByOffset.end();
	}

	// Add the number of free handles back to the heap.
	// This needs to be done before merging any blocks since merging
	// blocks modifies the numDescriptors variable.
	m_numFreeHandles += numDescriptors;

	if (prevBlockIt != m_freeListByOffset.end() && offset == prevBlockIt->first + prevBlockIt->second.Size) {
		// The previous block is exactly behind the block that is to be freed.
		//
		// PrevBlock.Offset           Offset
		// |                          |
		// |<-----PrevBlock.Size----->|<------Size-------->|
		//

		// Increase the block size by the size of merging with the previous block.
		offset = prevBlockIt->first;
		numDescriptors += prevBlockIt->second.Size;

		// Remove the previous block from the free list.
		m_freeListBySize.erase(prevBlockIt->second.FreeListBySizeIt);
		m_freeListByOffset.erase(prevBlockIt);
	}

	if (nextBlockIt != m_freeListByOffset.end() && offset + numDescriptors == nextBlockIt->first) {
		// The next block is exactly in front of the block that is to be freed.
		//
		// Offset               NextBlock.Offset
		// |                    |
		// |<------Size-------->|<-----NextBlock.Size----->|

		// Increase the block size by the size of merging with the next block.
		numDescriptors += nextBlockIt->second.Size;

		// Remove the next block from the free list.
		m_freeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
		m_freeListByOffset.erase(nextBlockIt);
	}

	// Add the freed block to the free list.
	AddNewBlock(offset, numDescriptors);
}

void CEDescriptorAllocatorPage::ReleaseStaleDescriptors() {
	std::lock_guard<std::mutex> lock(m_allocationMutex);

	while (!m_staleDescriptors.empty()) {
		auto& staleDescriptor = m_staleDescriptors.front();

		// The offset of the descriptor in the heap.
		auto offset = staleDescriptor.Offset;
		// The number of descriptors that were allocated.
		auto numDescriptors = staleDescriptor.Size;

		FreeBlock(offset, numDescriptors);

		m_staleDescriptors.pop();
	}
}
