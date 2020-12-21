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

private:
	struct CECommandAllocatorEntry {
		uint64_t fenceValue;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};
};
