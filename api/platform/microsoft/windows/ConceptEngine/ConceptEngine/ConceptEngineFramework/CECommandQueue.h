#pragma once
#include <atomic>
#include <d3d12.h>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <wrl.h>

#include "CEThreadSafeQueue.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEDevice;
	class CECommandList;
	namespace wrl = Microsoft::WRL;

	class CECommandQueue {
	public:
		/**
		 * Command List available for Command Queue
		 */
		std::shared_ptr<CECommandList> GetCommandList();

		/**
		 * Execute command list;
		 * Returns fence value to wait for command list
		 */
		uint64_t ExecuteCommandList(std::shared_ptr<CECommandList> commandList);
		uint64_t ExecuteCommandLists(const std::vector<std::shared_ptr<CECommandList>>& commandLists);

		uint64_t Signal();
		bool IsFenceComplete(uint64_t fenceValue);
		void WaitForFenceValue(uint64_t fenceValue);
		void Flush();

		/**
		 * Wait for another commandQueue to finish work;
		 */
		void Wait(const CECommandQueue& commandQueue);

		wrl::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

	protected:
		friend class std::default_delete<CECommandQueue>;

		CECommandQueue(CEDevice& device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CECommandQueue();

	private:
		/**
		 * Free command list that are finished processing on the command queue
		 */
		void ProcessCommandLists();

		/**
		 * Keep track of command allocators that are in use,
		 * first member is fence value to wait for,
		 * second is shared pointer to used command list;
		 */
		using CommandListEntry = std::tuple<uint64_t, std::shared_ptr<CECommandList>>;

		CEDevice& m_device;
		D3D12_COMMAND_LIST_TYPE m_commandListType;
		wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
		wrl::ComPtr<ID3D12Fence> m_fence;
		std::atomic_uint64_t m_fenceValue;

		CEThreadSafeQueue<CommandListEntry> m_inUseCommandLists;
		CEThreadSafeQueue<std::shared_ptr<CECommandList>> m_availableCommandLists;

		std::thread m_processInUseCommandListsThread;
		std::atomic_bool m_boolProcessInUseCommandLists;
		std::mutex m_processInUseCommandListsThreadMutex;
		std::condition_variable m_processInUseCommandListsThreadConditionVariable;
	};
}
