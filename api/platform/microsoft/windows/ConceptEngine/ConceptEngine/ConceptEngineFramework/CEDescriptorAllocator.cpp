#include "CEDescriptorAllocator.h"

#include "CEDescriptorAllocatorPage.h"
using namespace ConceptFramework::GraphicsEngine::Direct3D;

/*
 * Instance for shared_ptr
 */
struct CEAllocatorPageInstance : public CEDescriptorAllocatorPage {
public:
	CEAllocatorPageInstance(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
		: CEDescriptorAllocatorPage(device, type, numDescriptors) {
	}

	virtual ~CEAllocatorPageInstance() {
	};
};

CEDescriptorAllocator::CEDescriptorAllocator(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                             uint32_t numDescriptorsPerHeap)
	: m_device(device)
	  , m_heapType(type)
	  , m_numDescriptorsPerHeap(numDescriptorsPerHeap) {
}

CEDescriptorAllocator::~CEDescriptorAllocator() {
}

std::shared_ptr<CEDescriptorAllocatorPage> CEDescriptorAllocator::CreateAllocatorPage() {
	std::shared_ptr<CEDescriptorAllocatorPage> newPage = std::make_shared<CEAllocatorPageInstance>(
		m_device, m_heapType, m_numDescriptorsPerHeap);

	m_heapPool.emplace_back(newPage);
	m_availableHeaps.insert(m_heapPool.size() - 1);

	return newPage;
}

CEDescriptorAllocation CEDescriptorAllocator::Allocate(uint32_t numDescriptors) {
	std::lock_guard<std::mutex> lock(m_allocationMutex);

	CEDescriptorAllocation allocation;

	auto iter = m_availableHeaps.begin();
	while (iter != m_availableHeaps.end()) {
		auto allocatorPage = m_heapPool[*iter];

		allocation = allocatorPage->Allocate(numDescriptors);

		if (allocatorPage->NumFreeHandles() == 0) {
			iter = m_availableHeaps.erase(iter);
		}
		else {
			++iter;
		}

		// A valid allocation has been found.
		if (!allocation.IsNull()) {
			break;
		}
	}

	// No available heap could satisfy the requested number of descriptors.
	if (allocation.IsNull()) {
		m_numDescriptorsPerHeap = std::max(m_numDescriptorsPerHeap, numDescriptors);
		auto newPage = CreateAllocatorPage();

		allocation = newPage->Allocate(numDescriptors);
	}

	return allocation;
}

void CEDescriptorAllocator::ReleaseStaleDescriptors() {
	std::lock_guard<std::mutex> lock(m_allocationMutex);

	for (size_t i = 0; i < m_heapPool.size(); ++i) {
		auto page = m_heapPool[i];

		page->ReleaseStaleDescriptors();

		if (page->NumFreeHandles() > 0) {
			m_availableHeaps.insert(i);
		}
	}
}
