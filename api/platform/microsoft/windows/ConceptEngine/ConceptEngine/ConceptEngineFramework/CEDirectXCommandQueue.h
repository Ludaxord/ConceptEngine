#pragma once
#include <atomic>
#include <d3d12.h>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <wrl.h>

#include "CEDirectXThreadSafeQueue.h"

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXDevice;
	class CEDirectXCommandList;
	namespace wrl = Microsoft::WRL;

	class CEDirectXCommandQueue {
	public:
		/*
		 * Command List available for Command Queue
		 */
		std::shared_ptr<CEDirectXCommandList> GetCommandList();

		/*
		 * Execute command list;
		 * Returns fence value to wait for command list
		 */
		uint64_t ExecuteCommandList(std::shared_ptr<CEDirectXCommandList> commandList);
		uint64_t ExecuteCommandLists(const std::vector<std::shared_ptr<CEDirectXCommandList>>& commandLists);

		uint64_t Signal();
		bool IsFenceComplete(uint64_t fenceValue);
		void WaitForFenceValue(uint64_t fenceValue);
		void Flush();

		/*
		 * Wait for another commandQueue to finish work;
		 */
		void Wait(const CEDirectXCommandQueue& commandQueue);

		wrl::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

	protected:
		friend class std::default_delete<CEDirectXCommandQueue>;

		CEDirectXCommandQueue(CEDirectXDevice& device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CEDirectXCommandQueue();

	private:
		/*
		 * Free command list that are finished processing on the command queue
		 */
		void ProcessCommandLists();

		/*
		 * Keep track of command allocators that are in use,
		 * first member is fence value to wait for,
		 * second is shared pointer to used command list;
		 */
		using CommandListEntry = std::tuple<uint64_t, std::shared_ptr<CEDirectXCommandList>>;

		CEDirectXDevice& m_device;
		D3D12_COMMAND_LIST_TYPE m_commandListType;
		wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
		wrl::ComPtr<ID3D12Fence> m_fence;
		std::atomic_uint64_t m_fenceValue;

		CEDirectXThreadSafeQueue<CommandListEntry> m_inUseCommandLists;
		CEDirectXThreadSafeQueue<std::shared_ptr<CEDirectXCommandList>> m_availableCommandLists;

		std::thread m_processInUseCommandListsThread;
		std::atomic_bool m_boolProcessInUseCommandLists;
		std::mutex m_processInUseCommandListsThreadMutex;
		std::condition_variable m_processInUseCommandListsThreadConditionVariable;
	};
}
