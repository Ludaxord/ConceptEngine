#pragma once
#include <cstdint>
#include <d3d12.h>
#include <map>
#include <mutex>
#include <queue>
#include <wrl.h>


#include "CEDirectXDescriptorAllocation.h"
#include "d3dx12.h"

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	namespace wrl = Microsoft::WRL;
	class CEDirectXDevice;

	class CEDirectXDescriptorAllocatorPage : public std::enable_shared_from_this<CEDirectXDescriptorAllocatorPage> {
	public:
		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

		/*
		 * Check if descriptor page has block of descriptors
		 */
		bool HasSpace(uint32_t numDescriptors) const;

		/*
		 * Get number of available handles in the heap
		 */
		uint32_t NumFreeHandles() const;

		/*
		 * Allocate number of descriptors from descriptor heap
		 * If allocation cannot be satisfied (there is not enough space) then descriptor is set to NULL
		 */
		CEDirectXDescriptorAllocation Allocate(uint32_t numDescriptors);

		/*
		 * Return a descriptor back to heap.
		 */
		void Free(CEDirectXDescriptorAllocation&& descriptorHandle);

		/*
		 * Returned stale descriptor back to the descriptor heap
		 */
		void ReleaseStaleDescriptors();

	protected:
		CEDirectXDescriptorAllocatorPage(CEDirectXDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
		                                 uint32_t numDescriptors);
		virtual ~CEDirectXDescriptorAllocatorPage() = default;

		/*
		 * Compute offset of descriptor handle from the starto of the heap
		 */
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		/*
		 * Add new block to the free list
		 */
		void AddNewBlock(uint32_t offset, uint32_t numDescriptors);

		/*
		 * Free block of descriptors
		 * Merge free blocks to form larger blocks
		 */
		void FreeBlock(uint32_t offset, uint32_t numDescriptors);

	private:
		/*
		 * Offset type (in descriptors) within the descriptor heap
		 */
		using OffsetType = uint32_t;
		/*
		 * Number type of descriptors that are available
		 */
		using SizeType = uint32_t;

		struct FreeBlockInfo;
		/*
		 * Map of free blocks by the offset within the descriptor heap
		 */
		using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;

		/*
		 * Map of free block by size;
		 * Multimap because blocks can have same size;
		 */
		using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

		struct FreeBlockInfo {
			FreeBlockInfo(SizeType size): Size(size) {
			}

			SizeType Size;
			FreeListBySize::iterator FreeListBySize;
		};

		struct StaleDescriptorInfo {
			StaleDescriptorInfo(OffsetType offset, SizeType size): Offset(offset), Size(size) {

			}

			OffsetType Offset;
			SizeType Size;
		};

		CEDirectXDevice& m_device;

		/*
		 * Stale descriptors are queued for release until frame that they were freed has completed;
		 */
		using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

		FreeListByOffset m_freeListByOffset;
		FreeListBySize m_freeListBySize;
		StaleDescriptorQueue m_staleDescriptors;

		wrl::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_baseDescriptor;

		uint32_t m_descriptorHandleIncrementSize;
		uint32_t m_numFreeHandles;

		std::mutex m_allocationMutex;
	};
}
