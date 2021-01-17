#pragma once
#include <cstdint>
#include <d3d12.h>
#include <memory>


namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXDescriptorAllocatorPage;

	class CEDirectXDescriptorAllocation {
	public:
		CEDirectXDescriptorAllocation();
		CEDirectXDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles,
		                              std::shared_ptr<CEDirectXDescriptorAllocatorPage> page);
		~CEDirectXDescriptorAllocation();

		//Copies are not allowed
		CEDirectXDescriptorAllocation(const CEDirectXDescriptorAllocation&) = delete;
		CEDirectXDescriptorAllocation& operator=(const CEDirectXDescriptorAllocation&) = delete;

		//Move is allowed
		CEDirectXDescriptorAllocation(CEDirectXDescriptorAllocation&& allocation) noexcept;
		CEDirectXDescriptorAllocation& operator=(CEDirectXDescriptorAllocation other) noexcept;

		bool IsNull() const;

		bool isValid() const {
			return !IsNull();
		}

		/*
		 * Get descriptor at given offset in the allocation.
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;

		/*
		 * Get number of handles for this allocation.
		 */
		uint32_t GetNumHandles() const;

		/*
		 * Get heap that allocation came from
		 */
		std::shared_ptr<CEDirectXDescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

	private:
		/*
		 * Free descriptor back to the heap that came from
		 */
		void Free();

		/*
		 * Base Descriptor
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor;
		/*
		 * Number of descriptors in allocation
		 */
		uint32_t m_numHandles;
		/*
		 * offset to next descriptor
		 */
		uint32_t m_descriptorSize;
		/*
		 * pointer to the original page where allocation came from;
		 */
		std::shared_ptr<CEDirectXDescriptorAllocatorPage> m_page;
	};
}
