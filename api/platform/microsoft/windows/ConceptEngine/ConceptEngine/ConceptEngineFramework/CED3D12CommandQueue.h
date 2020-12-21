#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <queue>
namespace wrl = Microsoft::WRL;

class CED3D12CommandQueue {
public:
	CED3D12CommandQueue(wrl::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CED3D12CommandQueue();
	// Get an available command list from the command queue.
	wrl::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

	// Execute a command list.
	// Returns the fence value to wait for for this command list.
	uint64_t ExecuteCommandList(wrl::ComPtr<ID3D12GraphicsCommandList> commandList);

	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForPreviousFrame(uint64_t fenceValue);
protected:
	wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	wrl::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(wrl::ComPtr<ID3D12CommandAllocator> allocator);


	wrl::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;
private:
	struct CECommandAllocatorEntry {
		uint64_t fenceValue;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CECommandAllocatorEntry>;
	using CommandListQueue = std::queue<wrl::ComPtr<ID3D12GraphicsCommandList>>;

	D3D12_COMMAND_LIST_TYPE m_CommandListType;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;
	HANDLE m_FenceEvent;
	uint64_t m_FenceValue;

	CommandAllocatorQueue m_CommandAllocatorQueue;
	CommandListQueue m_CommandListQueue;
};
