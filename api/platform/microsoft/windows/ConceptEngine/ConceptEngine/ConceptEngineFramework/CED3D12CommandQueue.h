#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <queue>
namespace wrl = Microsoft::WRL;

class CED3D12CommandQueue {
public:
	CED3D12CommandQueue(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CED3D12CommandQueue();
public:
	wrl::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
	uint64_t ExecuteCommandList(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	wrl::ComPtr<ID3D12CommandQueue> GetCommandQueue() const;

protected:
	wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(); 
	wrl::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(wrl::ComPtr<ID3D12CommandAllocator> allocator);
private:
	struct CECommandAllocatorEntry {
		uint64_t fenceValue;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CECommandAllocatorEntry>;
	using CommandListQueue = std::queue<wrl::ComPtr<ID3D12GraphicsCommandList2>>;

	D3D12_COMMAND_LIST_TYPE m_commandListType;
	wrl::ComPtr<ID3D12Device2> m_d3d12Device;
	wrl::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
	wrl::ComPtr<ID3D12Fence> m_d3d12Fence;
	HANDLE m_fenceEvent;
	uint64_t m_fenceValue;
	UINT m_nodeMask;
	
	CommandAllocatorQueue m_CommandAllocatorQueue;
	CommandListQueue m_CommandListQueue;

};
