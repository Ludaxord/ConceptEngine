#pragma once
#include <mutex>
#include <set>
#include <vector>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CEDescriptorAllocatorPage;
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEDescriptorAllocator {
	public:
		/**
		 * Allocate number of contiguous descriptors from a CPU visible descriptor heap.
		 *
		 * @param numDescriptors, number of contiguous descriptors to allocate.
		 * Cannot be more than number of descriptors per descriptor heap.
		 */
		CEDescriptorAllocation Allocate(uint32_t numDescriptors = 1);

		/**
		 * When frame has completed, stale descriptors can be released.
		 */
		void ReleaseStaleDescriptors();

	protected:
		friend class std::default_delete<CEDescriptorAllocator>;

		CEDescriptorAllocator(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
		                      uint32_t numDescriptorsPerHeap = 256);
		virtual ~CEDescriptorAllocator();

	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<CEDescriptorAllocatorPage>>;

		/**
		 * Create new heap with specific number of descriptors
		 */
		std::shared_ptr<CEDescriptorAllocatorPage> CreateAllocatorPage();

		/**
		 * Device that was use to create DescriptorAllocator.
		 */
		CEDevice& m_device;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		uint32_t m_numDescriptorsPerHeap;

		DescriptorHeapPool m_heapPool;
		/**
		 * Indices of available heaps in heap pool;
		 */
		std::set<size_t> m_availableHeaps;

		std::mutex m_allocationMutex;
	};
}
