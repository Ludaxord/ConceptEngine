#pragma once
#include <mutex>
#include <set>
#include <vector>
#include <wrl.h>

#include "CEDirectXDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXDescriptorAllocatorPage;
	class CEDirectXDevice;
	namespace wrl = Microsoft::WRL;

	class CEDirectXDescriptorAllocator {

		/*
		 * Allocate number of contiguous descriptors from a CPU visible descriptor heap.
		 *
		 * @param numDescriptors, number of contiguous descriptors to allocate.
		 * Cannot be more than number of descriptors per descriptor heap.
		 */
		CEDirectXDescriptorAllocation Allocate(uint32_t numDescriptors = 1);

		/*
		 * When frame has completed, stale descriptors can be released.
		 */
		void ReleaseStaleDescriptors();

	protected:
		friend class std::default_delete<CEDirectXDescriptorAllocator>;

		CEDirectXDescriptorAllocator(CEDirectXDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
		                             uint32_t numDescriptorsPerHeap = 256);
		virtual ~CEDirectXDescriptorAllocator();

	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<CEDirectXDescriptorAllocatorPage>>;

		/*
		 * Create new heap with specific number of descriptors
		 */
		std::shared_ptr<CEDirectXDescriptorAllocatorPage> CreateAllocatorPage();

		/*
		 * Device that was use to create DescriptorAllocator.
		 */
		CEDirectXDevice& m_device;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		uint32_t m_numDescriptorsPerHeap;

		DescriptorHeapPool m_heapPool;
		/*
		 * Indices of available heaps in heap pool;
		 */
		std::set<size_t> m_availableHeaps;

		std::mutex m_allocationMutex;
	};
}
