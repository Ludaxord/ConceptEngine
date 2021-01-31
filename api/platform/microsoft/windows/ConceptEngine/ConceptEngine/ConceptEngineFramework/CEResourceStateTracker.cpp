#include "CEResourceStateTracker.h"

#include <cassert>


#include "CECommandList.h"
#include "CEResource.h"
#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

// Static definitions.
std::mutex CEResourceStateTracker::m_globalMutex;
bool CEResourceStateTracker::m_isLocked = false;
CEResourceStateTracker::ResourceStateMap CEResourceStateTracker::m_globalResourceState;
//ResourceStateTracker::ResourceList     ResourceStateTracker::ms_GarbageResources;

CEResourceStateTracker::CEResourceStateTracker() {
}

CEResourceStateTracker::~CEResourceStateTracker() {
}

void CEResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
	if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
		const D3D12_RESOURCE_TRANSITION_BARRIER& transitionBarrier = barrier.Transition;

		// First check if there is already a known "final" state for the given resource.
		// If there is, the resource has been used on the command list before and
		// already has a known state within the command list execution.
		const auto iter = m_finalResourceState.find(transitionBarrier.pResource);
		if (iter != m_finalResourceState.end()) {
			auto& resourceState = iter->second;
			// If the known final state of the resource is different...
			if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
				!resourceState.SubResourceState.empty()) {
				// First transition all of the subresources if they are different than the StateAfter.
				for (auto subresourceState : resourceState.SubResourceState) {
					if (transitionBarrier.StateAfter != subresourceState.second) {
						D3D12_RESOURCE_BARRIER newBarrier = barrier;
						newBarrier.Transition.Subresource = subresourceState.first;
						newBarrier.Transition.StateBefore = subresourceState.second;
						m_resourceBarriers.push_back(newBarrier);
					}
				}
			}
			else {
				auto finalState = resourceState.GetSubResourceState(transitionBarrier.Subresource);
				if (transitionBarrier.StateAfter != finalState) {
					// Push a new transition barrier with the correct before state.
					D3D12_RESOURCE_BARRIER newBarrier = barrier;
					newBarrier.Transition.StateBefore = finalState;
					m_resourceBarriers.push_back(newBarrier);
				}
			}
		}
		else // In this case, the resource is being used on the command list for the first time.
		{
			// Add a pending barrier. The pending barriers will be resolved
			// before the command list is executed on the command queue.
			m_pendingResourceBarriers.push_back(barrier);
		}

		// Push the final known state (possibly replacing the previously known state for the subresource).
		m_finalResourceState[transitionBarrier.pResource].SetSubResourceState(transitionBarrier.Subresource,
		                                                                      transitionBarrier.StateAfter);
	}
	else {
		// Just push non-transition barriers to the resource barriers array.
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
	ID3D12Resource* pResource = resource != nullptr ? resource->GetD3D12Resource().Get() : nullptr;

	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(pResource));
}

void CEResourceStateTracker::AliasBarrier(const CEResource* resourceBefore, const CEResource* resourceAfter) {
	ID3D12Resource* pResourceBefore = resourceBefore != nullptr ? resourceBefore->GetD3D12Resource().Get() : nullptr;
	ID3D12Resource* pResourceAfter = resourceAfter != nullptr ? resourceAfter->GetD3D12Resource().Get() : nullptr;

	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(pResourceBefore, pResourceAfter));
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

uint32_t CEResourceStateTracker::FlushPendingResourceBarriers(const std::shared_ptr<CECommandList>& commandList) {
	assert(m_isLocked);
	assert(commandList);

	// Resolve the pending resource barriers by checking the global state of the
	// (sub)resources. Add barriers if the pending state and the global state do
	//  not match.
	ResourceBarriers resourceBarriers;
	// Reserve enough space (worst-case, all pending barriers).
	resourceBarriers.reserve(m_pendingResourceBarriers.size());

	for (auto pendingBarrier : m_pendingResourceBarriers) {
		if (pendingBarrier.Type ==
			D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) // Only transition barriers should be pending...
		{
			auto pendingTransition = pendingBarrier.Transition;

			const auto& iter = m_globalResourceState.find(pendingTransition.pResource);
			if (iter != m_globalResourceState.end()) {
				// If all subresources are being transitioned, and there are multiple
				// subresources of the resource that are in a different state...
				auto& resourceState = iter->second;
				if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!resourceState.SubResourceState.empty()) {
					// Transition all subresources
					for (auto subresourceState : resourceState.SubResourceState) {
						if (pendingTransition.StateAfter != subresourceState.second) {
							D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
							newBarrier.Transition.Subresource = subresourceState.first;
							newBarrier.Transition.StateBefore = subresourceState.second;
							resourceBarriers.push_back(newBarrier);
						}
					}
				}
				else {
					// No (sub)resources need to be transitioned. Just add a single transition barrier (if needed).
					auto globalState = (iter->second).GetSubResourceState(pendingTransition.Subresource);
					if (pendingTransition.StateAfter != globalState) {
						// Fix-up the before state based on current global state of the resource.
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

void CEResourceStateTracker::CommitFinalResourceStates() {
	assert(m_isLocked);

	// Commit final resource states to the global resource state array (map).
	for (const auto& resourceState : m_finalResourceState) {
		m_globalResourceState[resourceState.first] = resourceState.second;
	}

	m_finalResourceState.clear();
}

void CEResourceStateTracker::Reset() {
	// Reset the pending, current, and final resource states.
	m_pendingResourceBarriers.clear();
	m_resourceBarriers.clear();
	m_finalResourceState.clear();

	//RemoveGarbageResources();
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
