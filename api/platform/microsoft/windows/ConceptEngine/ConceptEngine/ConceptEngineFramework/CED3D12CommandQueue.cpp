#include "CED3D12CommandQueue.h"

#include <cassert>


#include "CEHelper.h"


CED3D12CommandQueue::CED3D12CommandQueue(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type):
	m_fenceValue(0), m_commandListType(type), m_d3d12Device(device), m_nodeMask(0) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = m_nodeMask;

	ThrowIfFailed(m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue)));
	ThrowIfFailed(m_d3d12Device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence)));

	m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_fenceEvent && "Failed to create fence event handle.");
}

wrl::ComPtr<ID3D12CommandAllocator> CED3D12CommandQueue::CreateCommandAllocator() {
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	ThrowIfFailed(m_d3d12Device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&commandAllocator)));
	return commandAllocator;
}

wrl::ComPtr<ID3D12GraphicsCommandList2> CED3D12CommandQueue::CreateCommandList(
	wrl::ComPtr<ID3D12CommandAllocator> allocator) {
	wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
	ThrowIfFailed(m_d3d12Device->CreateCommandList(m_nodeMask, m_commandListType, allocator.Get(), nullptr,
	                                               IID_PPV_ARGS(&commandList)));
	return commandList;
}
