#include "CEResourceStateTracker.h"

#include <cassert>


#include "CECommandList.h"
#include "CEResource.h"
#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

/*
 * Static definitions
 */
std::mutex CEResourceStateTracker::m_globalMutex;
bool CEResourceStateTracker::m_isLocked = false;
CEResourceStateTracker::ResourceStateMap CEResourceStateTracker::m_globalResourceState;

CEResourceStateTracker::CEResourceStateTracker() {
}

CEResourceStateTracker::~CEResourceStateTracker() {
}

void CEResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
	if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
		const D3D12_RESOURCE_TRANSITION_BARRIER& transitionBarrier = barrier.Transition;

		/*
		 * First check if there is already a "final" state for given resource,
		 * If it is present, resource has been used on command list before and
		 * already has a known state within command list execution.
		 */
		const auto iter = m_finalResourceState.find(transitionBarrier.pResource);
		if (iter != m_finalResourceState.end()) {
			auto& resourceState = iter->second;
			/*
			 * if there is difference in resource final states
			 */
			if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resourceState.
				SubResourceState.empty()) {
				/*
				 * Transition all of subresources if they are different than stateAfter;
				 */
				for (auto subResourceState : resourceState.SubResourceState) {
					if (transitionBarrier.StateAfter != subResourceState.second) {
						D3D12_RESOURCE_BARRIER newBarrier = barrier;
						newBarrier.Transition.Subresource = subResourceState.first;
						newBarrier.Transition.StateBefore = subResourceState.second;
						m_resourceBarriers.push_back(newBarrier);
					}
				}
			}
			else {
				auto finalState = resourceState.GetSubResourceState(transitionBarrier.Subresource);
				if (transitionBarrier.StateAfter != finalState) {
					/*
					 * Push new transition barrier with correct before state.
					 */
					D3D12_RESOURCE_BARRIER newBarrier = barrier;
					newBarrier.Transition.StateBefore = finalState;
					m_resourceBarriers.push_back(newBarrier);
				}
			}
		}
		else {
			/*
			 * In case resource is being used on command list for first time
			 * Add pending barrier. Pending barriers will be resolved
			 * before command list is executed on command queue;
			 */
			m_pendingResourceBarriers.push_back(barrier);
		}

		/*
		 * Push final known state (possibly replacing replacing previously known state for subresource)
		 */
		m_finalResourceState[transitionBarrier.pResource].SetSubResourceState(
			transitionBarrier.Subresource, transitionBarrier.StateAfter);
	}
	else {
		/*
		 * Push non-transition barriers to resource barriers array.
		 */
		m_resourceBarriers.push_back(barrier);
	}
}

void CEResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter,
                                                UINT subResource) {
	if (resource) {
		ResourceBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
	}
}

void CEResourceStateTracker::TransitionResource(const CEResource& resource, D3D12_RESOURCE_STATES stateAfter,
                                                UINT subResource) {
	TransitionResource(resource.GetD3D12Resource().Get(), stateAfter, subResource);
}

void CEResourceStateTracker::UAVBarrier(const CEResource* resource) {
	ID3D12Resource* pointerResource = resource != nullptr ? resource->GetD3D12Resource().Get() : nullptr;
	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(pointerResource));
}

void CEResourceStateTracker::AliasBarrier(const CEResource* resourceBefore, const CEResource* resourceAfter) {
	ID3D12Resource* pointerResourceBefore =
		resourceBefore != nullptr ? resourceBefore->GetD3D12Resource().Get() : nullptr;
	ID3D12Resource* pointerResourceAfter =
		resourceAfter != nullptr ? resourceAfter->GetD3D12Resource().Get() : nullptr;
	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(pointerResourceBefore, pointerResourceAfter));
}

uint32_t CEResourceStateTracker::FlushPendingResourceBarriers(const std::shared_ptr<CECommandList>& commandList) {
	assert(m_isLocked);
	assert(commandList);

	/*
	 * resolve pending resource barriers by checking global state of (sub)resources.
	 * Add barriers if pending state and global state do not match.
	 */
	ResourceBarriers resourceBarriers;
	/*
	 * Reserve enough space (worst-case, all pending barriers).
	 */
	resourceBarriers.reserve(m_pendingResourceBarriers.size());

	for (auto pendingBarrier : m_pendingResourceBarriers) {
		/*
		 * Only transition barriers should be pending.
		 */
		if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
			auto pendingTransition = pendingBarrier.Transition;
			const auto& iter = m_globalResourceState.find(pendingTransition.pResource);
			if (iter != m_globalResourceState.end()) {
				/*
				 * If all subResources are being transitioned,
				 * and there are multiple subResources of resource that are in different state
				 */
				auto& resourceState = iter->second;
				if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resourceState.
					SubResourceState.empty()) {
					/*
					 * Transition all SubResources
					 */
					for (auto subResourceState : resourceState.SubResourceState) {
						if (pendingTransition.StateAfter != subResourceState.second) {
							D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
							newBarrier.Transition.Subresource = subResourceState.first;
							newBarrier.Transition.StateBefore = subResourceState.second;
							resourceBarriers.push_back(newBarrier);
						}
					}
				}
				else {
					/*
					 * No (sub) resources need to be transitioned, just add single transition barrier (if needed).
					 */
					auto globalState = (iter->second).GetSubResourceState(pendingTransition.Subresource);
					if (pendingTransition.StateAfter != globalState) {
						/*
						 * Fix-up before state based on current global state of resource.
						 */
						pendingBarrier.Transition.StateBefore = globalState;
						resourceBarriers.push_back(pendingBarrier);
					}
				}
			}
		}
	}

	UINT numBarriers = static_cast<UINT>(resourceBarriers.size());
	if (numBarriers > 0) {
		auto d3d12CommandList = commandList->GetCommandList();
		d3d12CommandList->ResourceBarrier(numBarriers, resourceBarriers.data());
	}

	m_pendingResourceBarriers.clear();

	return numBarriers;
}

void CEResourceStateTracker::FlushResourceBarriers(const std::shared_ptr<CECommandList>& commandList) {
	assert(commandList);

	UINT numBarriers = static_cast<UINT>(m_resourceBarriers.size());
	if (numBarriers > 0) {
		auto d3d12CommandList = commandList->GetCommandList();
		d3d12CommandList->ResourceBarrier(numBarriers, m_resourceBarriers.data());
		m_resourceBarriers.clear();
	}
}

void CEResourceStateTracker::CommitFinalResourceStates() {
	assert(m_isLocked);
	/*
	 * Commit final resource states to global resource state array (map).
	 */
	for (const auto& resourceState : m_finalResourceState) {
		m_globalResourceState[resourceState.first] = resourceState.second;
	}

	m_finalResourceState.clear();
}

void CEResourceStateTracker::Reset() {
	/*
	 * Reset pending, current and final resource states.
	 */
	m_pendingResourceBarriers.clear();
	m_resourceBarriers.clear();
	m_finalResourceState.clear();
}

void CEResourceStateTracker::Lock() {
	m_globalMutex.lock();
	m_isLocked = true;
}

void CEResourceStateTracker::Unlock() {
	m_globalMutex.unlock();
	m_isLocked = false;
}

void CEResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) {
	if (resource != nullptr) {
		std::lock_guard<std::mutex> lock(m_globalMutex);
		m_globalResourceState[resource].SetSubResourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
	}
}

/**
 * Check if resource is unique (only single strong reference)
 */
inline bool IsUnique(ID3D12Resource* resource) {
	resource->AddRef();
	return resource->Release() == 1;
}
