#include "CECommandQueue.h"

#include "CECommandList.h"
#include "CEDevice.h"
#include "CEHelper.h"
#include "CEResourceStateTracker.h"
using namespace Concept::GraphicsEngine::Direct3D;

/*
 * Instance for std::shared_ptr
 */
class CECommandListInstance : public CECommandList {

public:
	CECommandListInstance(CEDevice& device, D3D12_COMMAND_LIST_TYPE type)
		: CECommandList(device, type) {
	}

	virtual ~CECommandListInstance() {

	}
};

std::shared_ptr<CECommandList> CECommandQueue::GetCommandList() {
	std::shared_ptr<CECommandList> commandList;

	/*
	 * Get command list if there is any on queue.
	 */
	if (!m_availableCommandLists.Empty()) {
		m_availableCommandLists.TryPop(commandList);
	}
	else {
		/*
		 * If there are not any command lists available, create new one.
		 */
		commandList = std::make_shared<CECommandListInstance>(m_device, m_commandListType);
	}

	return commandList;
}

/**
 * Execute command list,
 * @returns fence value to wait for command list.
 */
uint64_t CECommandQueue::ExecuteCommandList(std::shared_ptr<CECommandList> commandList) {
	return ExecuteCommandLists(std::vector<std::shared_ptr<CECommandList>>({commandList}));
}

uint64_t CECommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CECommandList>>& commandLists) {
	CEResourceStateTracker::Lock();

	/*
	 * Command list that need to put back on command list queue.
	 */
	std::vector<std::shared_ptr<CECommandList>> executionCommandList;
	/*
	 * reserve space of vector to double of size commandLists, executionCommandList will have pending command lists
	 */
	executionCommandList.reserve(commandLists.size() * 2);

	/*
	 * Generate mips command lists.
	 */
	std::vector<std::shared_ptr<CECommandList>> generateMipsCommandLists;
	generateMipsCommandLists.reserve(commandLists.size());

	/*
	 * Command lists for executions 
	 */
	std::vector<ID3D12CommandList*> d3d12CommandLists;
	/*
	* reserve space of vector to double of size commandLists, executionCommandList will have pending command lists
	*/
	d3d12CommandLists.reserve(commandLists.size() * 2);

	for (auto commandList : commandLists) {
		auto pendingCommandList = GetCommandList();
		bool hasPendingBarriers = commandList->Close(pendingCommandList);
		pendingCommandList->Close();
		/*
		 * if there are no pending barriers on pending command list,
		 * there is no reason to execute an empty command list.
		 */
		if (hasPendingBarriers) {
			d3d12CommandLists.push_back(pendingCommandList->GetCommandList().Get());
		}
		d3d12CommandLists.push_back(commandList->GetCommandList().Get());

		executionCommandList.push_back(pendingCommandList);
		executionCommandList.push_back(commandList);

		auto generateMipsCommandList = commandList->GetGenerateMipsCommandList();
		if (generateMipsCommandList) {
			generateMipsCommandLists.push_back(generateMipsCommandList);
		}
	}

	UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());
	m_commandQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
	uint64_t fenceValue = Signal();

	CEResourceStateTracker::Unlock();

	/*
	 * Queue command lists for reuse.
	 */
	for (auto commandList : executionCommandList) {
		m_inUseCommandLists.Push({fenceValue, commandList});
	}

	/*
	 * If there are any command lists that generate mips then execute it
	 * after initial resource command lists have finished.
	 */
	if (generateMipsCommandLists.size() > 0) {
		auto& computeQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		computeQueue.Wait(*this);
		computeQueue.ExecuteCommandLists(generateMipsCommandLists);
	}

	return fenceValue;
}

uint64_t CECommandQueue::Signal() {
	uint64_t fenceValue = ++m_fenceValue;
	m_commandQueue->Signal(m_fence.Get(), fenceValue);
	return fenceValue;
}

bool CECommandQueue::IsFenceComplete(uint64_t fenceValue) {
	return m_fence->GetCompletedValue() >= fenceValue;
}

void CECommandQueue::WaitForFenceValue(uint64_t fenceValue) {
	if (!IsFenceComplete(fenceValue)) {
		auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (event) {
			m_fence->SetEventOnCompletion(fenceValue, event);
			::WaitForSingleObject(event, DWORD_MAX);

			::CloseHandle(event);
		}
	}
}

void CECommandQueue::Flush() {
	std::unique_lock<std::mutex> lock(m_processInUseCommandListsThreadMutex);
	m_processInUseCommandListsThreadConditionVariable.wait(lock, [this] { return m_inUseCommandLists.Empty(); });

	/*
	 * In case command queue was signaled directly
	 * using CommandQueue::Signal method then
	 * fence value of command queue might be higher than fence
	 * value of any of executed command lists.
	 */
	WaitForFenceValue(m_fenceValue);
}

void CECommandQueue::Wait(const CECommandQueue& commandQueue) {
	m_commandQueue->Wait(commandQueue.m_fence.Get(), commandQueue.m_fenceValue);
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CECommandQueue::GetD3D12CommandQueue() const {
	return m_commandQueue;
}

CECommandQueue::CECommandQueue(CEDevice& device, D3D12_COMMAND_LIST_TYPE type):
	m_device(device),
	m_commandListType(type),
	m_fenceValue(0),
	m_boolProcessInUseCommandLists(true) {
	auto d3d12Device = m_device.GetDevice();
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));;
	ThrowIfFailed(d3d12Device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	switch (type) {
	case D3D12_COMMAND_LIST_TYPE_COPY:
		m_commandQueue->SetName(L"Copy Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		m_commandQueue->SetName(L"Compute Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		m_commandQueue->SetName(L"Direct Command Queue");
		break;
	}

	/*
	 * Set thread name for easy debugging;
	 */
	char threadName[256];
	sprintf_s(threadName, "ProcessInUseCommandLists");
	switch (type) {
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		strcat_s(threadName, "(Direct)");
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		strcat_s(threadName, "(Compute)");
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		strcat_s(threadName, "(Copy)");
		break;
	default:
		break;
	}

	m_processInUseCommandListsThread = std::thread(&CECommandQueue::ProcessInUseCommandLists, this);
	SetThreadName(m_processInUseCommandListsThread, threadName);
}

CECommandQueue::~CECommandQueue() {
	m_boolProcessInUseCommandLists = false;
	m_processInUseCommandListsThread.join();
}

void CECommandQueue::ProcessInUseCommandLists() {
	std::unique_lock<std::mutex> lock(m_processInUseCommandListsThreadMutex, std::defer_lock);
	while (m_boolProcessInUseCommandLists) {
		CommandListEntry commandListEntry;
		lock.lock();
		while (m_inUseCommandLists.TryPop(commandListEntry)) {
			auto fenceValue = std::get<0>(commandListEntry);
			auto commandList = std::get<1>(commandListEntry);

			WaitForFenceValue(fenceValue);

			commandList->Reset();

			m_availableCommandLists.Push(commandList);
		}
		lock.unlock();
		m_processInUseCommandListsThreadConditionVariable.notify_one();
		std::this_thread::yield();
	}
}
