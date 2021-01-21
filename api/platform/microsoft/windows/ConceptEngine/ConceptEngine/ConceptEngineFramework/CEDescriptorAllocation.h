#pragma once
#include <cstdint>
#include <d3d12.h>
#include <memory>


namespace Concept::GraphicsEngine::Direct3D {
	class CEDescriptorAllocatorPage;

	class CEDescriptorAllocation {
	public:
		CEDescriptorAllocation();
		CEDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles, uint32_t descriptorSize,
		                       std::shared_ptr<CEDescriptorAllocatorPage> page);
		~CEDescriptorAllocation();

		//Copies are not allowed
		CEDescriptorAllocation(const CEDescriptorAllocation&) = delete;
		CEDescriptorAllocation& operator=(const CEDescriptorAllocation&) = delete;

		//Move is allowed
		CEDescriptorAllocation(CEDescriptorAllocation&& allocation) noexcept;
		CEDescriptorAllocation& operator=(CEDescriptorAllocation&& other) noexcept;

		bool IsNull() const;

		bool isValid() const {
			return !IsNull();
		}

		/**
		 * Get descriptor at given offset in the allocation.
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;

		/**
		 * Get number of handles for this allocation.
		 */
		uint32_t GetNumHandles() const;

		/**
		 * Get heap that allocation came from
		 */
		std::shared_ptr<CEDescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

	private:
		/**
		 * Free descriptor back to the heap that came from
		 */
		void Free();

		/**
		 * Base Descriptor
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor;
		/**
		 * Number of descriptors in allocation
		 */
		uint32_t m_numHandles;
		/**
		 * offset to next descriptor
		 */
		uint32_t m_descriptorSize;
		/**
		 * pointer to the original page where allocation came from;
		 */
		std::shared_ptr<CEDescriptorAllocatorPage> m_page;
	};
}
