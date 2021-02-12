#include "CEUploadBuffer.h"


#include "CEDevice.h"
#include "CEHelper.h"
#include "d3dx12.h"
using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEUploadBuffer::CEUploadBuffer(CEDevice& device, size_t pageSize)
	: m_device(device)
	  , m_pageSize(pageSize) {
}

CEUploadBuffer::~CEUploadBuffer() {
}

CEUploadBuffer::Allocation CEUploadBuffer::Allocate(size_t sizeInBytes, size_t alignment) {
	if (sizeInBytes > m_pageSize) {
		throw std::bad_alloc();
	}

	// If there is no current page, or the requested allocation exceeds the
	// remaining space in the current page, request a new page.
	if (!m_currentPage || !m_currentPage->HasSpace(sizeInBytes, alignment)) {
		m_currentPage = RequestPage();
	}

	return m_currentPage->Allocate(sizeInBytes, alignment);
}

std::shared_ptr<CEUploadBuffer::Page> CEUploadBuffer::RequestPage() {
	std::shared_ptr<Page> page;

	if (!m_availablePages.empty()) {
		page = m_availablePages.front();
		m_availablePages.pop_front();
	}
	else {
		page = std::make_shared<Page>(m_device, m_pageSize);
		m_pagePool.push_back(page);
	}

	return page;
}

void CEUploadBuffer::Reset() {
	m_currentPage = nullptr;
	// Reset all available pages.
	m_availablePages = m_pagePool;

	for (auto page : m_availablePages) {
		// Reset the page for new allocations.
		page->Reset();
	}
}

CEUploadBuffer::Page::Page(CEDevice& device, size_t sizeInBytes)
	: m_device(device)
	  , m_pageSize(sizeInBytes)
	  , m_offset(0)
	  , m_CPUPointer(nullptr)
	  , m_GPUPointer(D3D12_GPU_VIRTUAL_ADDRESS(0)) {
	auto d3d12Device = m_device.GetDevice();

	ThrowIfFailed(d3d12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_pageSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&m_resource)));

	m_resource->SetName(L"Upload Buffer (Page)");

	m_GPUPointer = m_resource->GetGPUVirtualAddress();
	m_resource->Map(0, nullptr, &m_CPUPointer);
}

CEUploadBuffer::Page::~Page() {
	m_resource->Unmap(0, nullptr);
	m_CPUPointer = nullptr;
	m_GPUPointer = D3D12_GPU_VIRTUAL_ADDRESS(0);
}

bool CEUploadBuffer::Page::HasSpace(size_t sizeInBytes, size_t alignment) const {
	size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
	size_t alignedOffset = Math::AlignUp(m_offset, alignment);

	return alignedOffset + alignedSize <= m_pageSize;
}

CEUploadBuffer::Allocation CEUploadBuffer::Page::Allocate(size_t sizeInBytes, size_t alignment) {
	if (!HasSpace(sizeInBytes, alignment)) {
		// Can't allocate space from page.
		throw std::bad_alloc();
	}

	size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
	m_offset = Math::AlignUp(m_offset, alignment);

	Allocation allocation;
	allocation.CPU = static_cast<uint8_t*>(m_CPUPointer) + m_offset;
	allocation.GPU = m_GPUPointer + m_offset;

	m_offset += alignedSize;

	return allocation;
}

void CEUploadBuffer::Page::Reset() {
	m_offset = 0;
}
