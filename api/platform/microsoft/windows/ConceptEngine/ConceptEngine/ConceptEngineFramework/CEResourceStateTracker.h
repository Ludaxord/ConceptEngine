#pragma once
#include <d3d12.h>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <wrl.h>

namespace ConceptFramework::GraphicsEngine::Direct3D {
	class CECommandList;
	class CEResource;
	namespace wrl = Microsoft::WRL;

	class CEResourceStateTracker {
	public:
		CEResourceStateTracker();
		virtual ~CEResourceStateTracker();

		/**
		 * push resource barrier to resource state tracker.
		 *
		 * @param barrier, resource barrier to push to resource state tracker.
		 */
		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

		/**
		 * Push transition resource barrier to resource state tracker.
		 *
		 * @param resource, resource to transition.
		 * @param stateAfter, state to transition resource to.
		 * @param subResource, subResource to transition, By default it is set to D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		 * which indicates that all subResources should be transitioned to same state.
		 */
		void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter,
		                        UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void TransitionResource(const CEResource& resource, D3D12_RESOURCE_STATES stateAfter,
		                        UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Push UAV resource barrier for given resource.
		 *
		 * @param resource, resource to add UAV barrier for,
		 * can be NULL which indicates that any UAV could require barrier.
		 */
		void UAVBarrier(const CEResource* resource = nullptr);

		/**
		 * Push aliasing barrier for given resource.
		 *
		 * @param beforeResource, resource currently occupying space in heap.
		 * @param afterResource, resource that will be occupying space in heap.
		 *
		 * Either beforeResource or afterResource parameters can be NUL which indicates that
		 * any placed or reserved resource could case aliasing.
		 */
		void AliasBarrier(const CEResource* resourceBefore = nullptr,
		                  const CEResource* resourceAfter = nullptr);

		/**
		 * Flush any pending resource barriers to command list.
		 *
		 * @return number of resource barriers that were flushed to command list.
		 */
		uint32_t FlushPendingResourceBarriers(const std::shared_ptr<CECommandList>& commandList);

		/**
		 * Flush any (non-pending) resource barriers that has been pushed to resource state tracker
		 */
		void FlushResourceBarriers(const std::shared_ptr<CECommandList>& commandList);

		/**
		 * Commit final resource states to global resource state map.
		 * must be called when command list is closed.
		 */
		void CommitFinalResourceStates();

		/**
		 * Reset state tracking. This must be done when command list is reset.
		 */
		void Reset();

		/**
		 * global state must be locked before flushing pending resource barriers
		 * and committing final resource state to global resource state.
		 * Ensures consistency of global resource state between command list executions.
		 */
		static void Lock();

		/**
		 * Unlocks global resource state after final states has been committed to global resource state array.
		 */
		static void Unlock();

		/**
		 * Add resource with given state to global resource state array (map)
		 * Should be done when resource is created for first time.
		 */
		static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);

	protected:
	private:
		/**
		 * List (vector) of resource barriers.
		 */
		using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

		/**
		 * Pending resource transitions are committed before a command list
		 * is executed on command queue.
		 * Guarantees that resource will be in expected state at beginning of command list
		 */
		ResourceBarriers m_pendingResourceBarriers;

		/**
		 * Resource barriers that need to be committed to command list;
		 */
		ResourceBarriers m_resourceBarriers;

		/**
		 * Track state of particular resource and all of its subResources.
		 */
		struct ResourceState {
			/**
			 * Initialize all subResources within a resource to given state.
			 */
			explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON): State(state) {

			}

			/**
			 * Set subResource to particular state.
			 */
			void SetSubResourceState(UINT subResource, D3D12_RESOURCE_STATES state) {
				if (subResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
					State = state;
					SubResourceState.clear();
				}
				else {
					SubResourceState[subResource] = state;
				}
			}

			/**
			 * Get state of (sub)resource within resource.
			 * if specified subResource is not found in SubResourceStateArray (map)
			 * then state of resource (D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) is returned.
			 */
			D3D12_RESOURCE_STATES GetSubResourceState(UINT subResource) const {
				D3D12_RESOURCE_STATES state = State;
				const auto iter = SubResourceState.find(subResource);
				if (iter != SubResourceState.end()) {
					state = iter->second;
				}
				return state;
			}

			/**
			 * If SubResourceState array (map) is empty, then state variable defines
			 * state of all of subResources.
			 */
			D3D12_RESOURCE_STATES State;
			std::map<UINT, D3D12_RESOURCE_STATES> SubResourceState;
		};

		using ResourceList = std::vector<ID3D12Resource*>;
		using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

		/**
		 * Last known state of resources within command list.
		 * final resource state is committed to global resource state when
		 * command list is closed but before it is executed on command queue.
		 */
		ResourceStateMap m_finalResourceState;

		/**
		 * Global resource state array (map) stores state of resource
		 * between command list execution.
		 */
		static ResourceStateMap m_globalResourceState;

		/**
		 * mutex protects shared access to GlobalResourceState map
		 */
		static std::mutex m_globalMutex;
		static bool m_isLocked;
	};
}
