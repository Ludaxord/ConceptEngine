#include "CED3D12CommandQueue.h"

#include "CEHelper.h"


CED3D12CommandQueue::CED3D12CommandQueue(wrl::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type):
	m_FenceValue(1), m_CommandListType(type), m_d3d12Device(device) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = type;
	queueDesc.NodeMask = 0;

	ThrowIfFailed(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_d3d12CommandQueue)));
	ThrowIfFailed(m_d3d12Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence)));

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr) {
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

wrl::ComPtr<ID3D12GraphicsCommandList> CED3D12CommandQueue::GetCommandList() {
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	wrl::ComPtr<ID3D12GraphicsCommandList> commandList;
	if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue)) {
		commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
		m_CommandAllocatorQueue.pop();
	}
	else {
		commandAllocator = CreateCommandAllocator();
	}

	if (!m_CommandListQueue.empty()) {
		commandList = m_CommandListQueue.front();
		m_CommandListQueue.pop();
		ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
	}
	else {
		commandList = CreateCommandList(commandAllocator);
	}
	ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

	return commandList;
}

uint64_t CED3D12CommandQueue::Signal() {
	uint64_t fenceValueForSignal = ++m_FenceValue;
	ThrowIfFailed(m_d3d12CommandQueue->Signal(m_d3d12Fence.Get(), fenceValueForSignal));
	return fenceValueForSignal;
}

void CED3D12CommandQueue::Flush() {
}

uint64_t CED3D12CommandQueue::ExecuteCommandList(wrl::ComPtr<ID3D12GraphicsCommandList> commandList) {
	// commandList->Close();

	ID3D12CommandAllocator* commandAllocator;
	UINT dataSize = sizeof(commandAllocator);
	ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));
	ID3D12CommandList* ppCommandLists[] = {commandList.Get()};
	m_d3d12CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//TODO: Test if works; if not use Present before
	uint64_t fenceValue = Signal();
	m_CommandAllocatorQueue.emplace(CECommandAllocatorEntry{fenceValue, commandAllocator});
	m_CommandListQueue.push(commandList);
	commandAllocator->Release();
	return fenceValue;
}


bool CED3D12CommandQueue::IsFenceComplete(uint64_t fenceValue) {
	return m_d3d12Fence->GetCompletedValue() >= fenceValue;
}

void CED3D12CommandQueue::WaitForPreviousFrame(uint64_t fenceValue) {
}

wrl::ComPtr<ID3D12CommandAllocator> CED3D12CommandQueue::CreateCommandAllocator() {
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	ThrowIfFailed(m_d3d12Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator)));
	return commandAllocator;
}

wrl::ComPtr<ID3D12GraphicsCommandList> CED3D12CommandQueue::CreateCommandList(
	wrl::ComPtr<ID3D12CommandAllocator> allocator) {
	wrl::ComPtr<ID3D12GraphicsCommandList> commandList;
	ThrowIfFailed(m_d3d12Device->CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr,
	                                               IID_PPV_ARGS(&commandList)));
	ThrowIfFailed(commandList->Close());
	return commandList;
}

wrl::ComPtr<ID3D12CommandQueue> CED3D12CommandQueue::GetD3D12CommandQueue() const {
	wrl::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.NodeMask = 0;

	ThrowIfFailed(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3d12CommandQueue)));
	return d3d12CommandQueue;
}
