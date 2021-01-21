#include "CEDescriptorAllocatorPage.h"

#include "CEDevice.h"
#include "CEHelper.h"
using namespace Concept::GraphicsEngine::Direct3D;

D3D12_DESCRIPTOR_HEAP_TYPE CEDescriptorAllocatorPage::GetHeapType() const {
	return m_heapType;
}

bool CEDescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const {
	return m_freeListBySize.lower_bound(numDescriptors) != m_freeListBySize.end();
}

uint32_t CEDescriptorAllocatorPage::NumFreeHandles() const {
	return m_numFreeHandles;
}

CEDescriptorAllocation CEDescriptorAllocatorPage::Allocate(uint32_t numDescriptors) {
	std::lock_guard<std::mutex> lock(m_allocationMutex);

	/*
	 * There are less than requested number of descriptors lef in heap.
	 * return NULL descriptor and try another heap
	 */
	if (numDescriptors > m_numFreeHandles) {
		return CEDescriptorAllocation();
	}

	/*
	 * Get first block that is large enough to satisfy request.
	 */
	auto smallestBlockIt = m_freeListBySize.lower_bound(numDescriptors);
	if (smallestBlockIt == m_freeListBySize.end()) {
		/*
		 * There was no free block that could satisfy request
		 */
		return CEDescriptorAllocation();
	}

	/*
	 * Size of smallest block that satisfies request.
	 */
	auto blockSize = smallestBlockIt->first;

	/*
	 * pointer to same entry in FreeListByOffset map.
	 */
	auto offsetIt = smallestBlockIt->second;

	/*
	 * offset in descriptor heap
	 */
	auto offset = offsetIt->first;

	/*
	 * Remove existing free block frm free list.
	 */
	m_freeListBySize.erase(smallestBlockIt);
	m_freeListByOffset.erase(offsetIt);

	/*
	 * Compute new free block results from splitting block
	 */
	auto computedOffset = offset + numDescriptors;
	auto computedSize = blockSize - numDescriptors;

	if (computedSize > 0) {
		/*
		 * If allocation did not exactly match requested size,
		 * return left-over to free list.
		 */
		AddNewBlock(computedOffset, computedSize);
	}

	/*
	 * Decrement free handles
	 */
	m_numFreeHandles -= numDescriptors;

	return CEDescriptorAllocation(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_baseDescriptor, offset, m_descriptorHandleIncrementSize), numDescriptors,
		m_descriptorHandleIncrementSize, shared_from_this());
}

void CEDescriptorAllocatorPage::Free(CEDescriptorAllocation&& descriptorHandle) {
	/*
	 * Compute offset of descriptor within descriptor heap
	 */
	auto offset = ComputeOffset(descriptorHandle.GetDescriptorHandle());
	std::lock_guard<std::mutex> lock(m_allocationMutex);
	/*
	 * do not add block directly to free list until frame has completed.
	 */
	m_staleDescriptors.emplace(offset, descriptorHandle.GetNumHandles());
}

void CEDescriptorAllocatorPage::ReleaseStaleDescriptors() {
	std::lock_guard<std::mutex> lock(m_allocationMutex);
	while (!m_staleDescriptors.empty()) {
		auto& staleDescriptor = m_staleDescriptors.front();
		/*
		 * offset of descriptor in heap
		 */
		auto offset = staleDescriptor.Offset;
		/*
		 * Number of descriptors that were allocated
		 */
		auto numDescriptors = staleDescriptor.Size;

		FreeBlock(offset, numDescriptors);

		m_staleDescriptors.pop();
	}
}

CEDescriptorAllocatorPage::CEDescriptorAllocatorPage(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                     uint32_t numDescriptors): m_device(device),
                                                                               m_heapType(type),
                                                                               m_numDescriptorsInHeap(numDescriptors) {
	auto d3d12Device = m_device.GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = m_heapType;
	heapDesc.NumDescriptors = m_numDescriptorsInHeap;

	ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)));

	m_baseDescriptor = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_descriptorHandleIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(m_heapType);
	m_numFreeHandles = m_numDescriptorsInHeap;

	/*
	 * Initialize free lists
	 */
	AddNewBlock(0, m_numFreeHandles);
}

uint32_t CEDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	return static_cast<uint32_t>(handle.ptr - m_baseDescriptor.ptr) / m_descriptorHandleIncrementSize;
}

void CEDescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors) {
	auto offsetIt = m_freeListByOffset.emplace(offset, numDescriptors);
	auto sizeIt = m_freeListBySize.emplace(numDescriptors, offsetIt.first);
	offsetIt.first->second.FreeListBySizeIt = sizeIt;
}

void CEDescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors) {
	/*
	 * Find first element whose offset is greater than specified offset.
	 * This is block that should appear after block that is being freed.
	 */
	auto nextBlockIt = m_freeListByOffset.upper_bound(offset);

	/*
	 * find block that appears before block being freed
	 */
	auto prevBlockIt = nextBlockIt;
	/*
	 * if it is not first block in list
	 */
	if (prevBlockIt != m_freeListByOffset.begin()) {
		/*
		 * Go to previous block in list
		 */
		--prevBlockIt;
	}
	else {
		/*
		 * Otherwise, just set it to end of list to indicate
		 * that no block comes before one being freed.
		 */
		prevBlockIt = m_freeListByOffset.end();
	}

	/*
	 * Add number of free handles back to heap.
	 * This need to be done before merging any blocks since merging
	 * block modifies numDescriptors variable
	 */
	m_numFreeHandles += numDescriptors;

	if (prevBlockIt != m_freeListByOffset.end() && offset == prevBlockIt->first + prevBlockIt->second.Size) {
		/*
		 * Previous block is exactly behind block that is going to be freed
		 *
		 * PrevBlock.Offset			Offset
		 * |						|				   |
		 * |<---PrevBlock.Size----->|<------Size------>|
		 */

		/*
		 * Increase block size by size of merging with previous block
		 */
		offset = prevBlockIt->first;
		numDescriptors += prevBlockIt->second.Size;

		/*
		 * Remove previous block from free list
		 */
		m_freeListBySize.erase(prevBlockIt->second.FreeListBySizeIt);
		m_freeListByOffset.erase(prevBlockIt);
	}

	if (nextBlockIt != m_freeListByOffset.end() && offset + numDescriptors == nextBlockIt->first) {
		/*
		 * new block is exactly in from of block that is going to be freed
		 *
		 * Offset			NextBlock.Offset
		 * |				|						   |
		 * |<----Size------>|<-----NextBlock.Size----->|
		 */

		/*
		 * increase block size by size of merging with next block
		 */
		numDescriptors += nextBlockIt->second.Size;

		/*
		 * Remove next block from free list.
		 */
		m_freeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
		m_freeListByOffset.erase(nextBlockIt);
	}

	/*
	 * Add freed block to free list
	 */
	AddNewBlock(offset, numDescriptors);
}
