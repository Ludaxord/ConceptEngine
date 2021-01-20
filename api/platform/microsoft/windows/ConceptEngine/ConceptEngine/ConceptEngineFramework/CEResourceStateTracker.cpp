#include "CEResourceStateTracker.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEResourceStateTracker::CEResourceStateTracker() {
}

CEResourceStateTracker::~CEResourceStateTracker() {
}

void CEResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
}

void CEResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter,
	UINT subResource) {
}

void CEResourceStateTracker::TransitionResource(const CEResource& resource, D3D12_RESOURCE_STATES stateAfter,
	UINT subResource) {
}

void CEResourceStateTracker::UAVBarrier(const CEResource* resource) {
}

void CEResourceStateTracker::AliasBarrier(const CEResource* resourceBefore, const CEResource* resourceAfter) {
}

uint32_t CEResourceStateTracker::FlushPendingResourceBarriers(const std::shared_ptr<CECommandList>& commandList) {
}

void CEResourceStateTracker::FlushResourceBarriers(const std::shared_ptr<CECommandList>& commandList) {
}

void CEResourceStateTracker::CommitFinalResourceStates() {
}

void CEResourceStateTracker::Reset() {
}

void CEResourceStateTracker::Lock() {
}

void CEResourceStateTracker::Unlock() {
}

void CEResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) {
}

void CEResourceStateTracker::RemoveGlobalResourceState(ID3D12Resource* resource, bool immediate) {
}

void CEResourceStateTracker::RemoveGarbageResources() {
}
