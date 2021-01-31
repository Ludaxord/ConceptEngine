#include "CEDescriptorAllocation.h"

#include <cassert>


#include "CEDescriptorAllocatorPage.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEDescriptorAllocation::CEDescriptorAllocation()
	: m_descriptor{0}
	  , m_numHandles(0)
	  , m_descriptorSize(0)
	  , m_page(nullptr) {
}

CEDescriptorAllocation::CEDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles,
                                               uint32_t descriptorSize, std::shared_ptr<CEDescriptorAllocatorPage> page)
	: m_descriptor(descriptor)
	  , m_numHandles(numHandles)
	  , m_descriptorSize(descriptorSize)
	  , m_page(page) {
}


CEDescriptorAllocation::~CEDescriptorAllocation() {
	Free();
}

CEDescriptorAllocation::CEDescriptorAllocation(CEDescriptorAllocation&& allocation) noexcept
	: m_descriptor(allocation.m_descriptor)
	  , m_numHandles(allocation.m_numHandles)
	  , m_descriptorSize(allocation.m_descriptorSize)
	  , m_page(std::move(allocation.m_page)) {
	allocation.m_descriptor.ptr = 0;
	allocation.m_numHandles = 0;
	allocation.m_descriptorSize = 0;
}

CEDescriptorAllocation& CEDescriptorAllocation::operator=(CEDescriptorAllocation&& other) noexcept {
	// Free this descriptor if it points to anything.
	Free();

	m_descriptor = other.m_descriptor;
	m_numHandles = other.m_numHandles;
	m_descriptorSize = other.m_descriptorSize;
	m_page = std::move(other.m_page);

	other.m_descriptor.ptr = 0;
	other.m_numHandles = 0;
	other.m_descriptorSize = 0;

	return *this;
}

void CEDescriptorAllocation::Free() {
	if (!IsNull() && m_page) {
		m_page->Free(std::move(*this));

		m_descriptor.ptr = 0;
		m_numHandles = 0;
		m_descriptorSize = 0;
		m_page.reset();
	}
}

// Check if this a valid descriptor.
bool CEDescriptorAllocation::IsNull() const {
	return m_descriptor.ptr == 0;
}

// Get a descriptor at a particular offset in the allocation.
D3D12_CPU_DESCRIPTOR_HANDLE CEDescriptorAllocation::GetDescriptorHandle(uint32_t offset) const {
	assert(offset < m_numHandles);
	return {m_descriptor.ptr + (m_descriptorSize * offset)};
}

uint32_t CEDescriptorAllocation::GetNumHandles() const {
	return m_numHandles;
}

std::shared_ptr<CEDescriptorAllocatorPage> CEDescriptorAllocation::GetDescriptorAllocatorPage() const {
	return m_page;
}
