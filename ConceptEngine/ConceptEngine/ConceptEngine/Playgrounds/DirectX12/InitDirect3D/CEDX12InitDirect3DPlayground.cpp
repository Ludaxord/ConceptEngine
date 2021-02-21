#include "CEDX12InitDirect3DPlayground.h"

#include <DirectXColors.h>

#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/d3dx12.h"
#include "../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12InitDirect3DPlayground::CEDX12InitDirect3DPlayground(): CEDX12Playground() {
}

void CEDX12InitDirect3DPlayground::Update(const CETimer& gt) {
}

void CEDX12InitDirect3DPlayground::Render(const CETimer& gt) {

	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandAllocator = m_dx12manager->GetD3D12CommandAllocator();
	auto commandList = m_dx12manager->GetD3D12CommandList();
	auto swapChain = m_dx12manager->GetDXGISwapChain();

	auto screenViewport = m_dx12manager->GetViewPort();
	auto scissorRect = m_dx12manager->GetScissorRect();

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(commandAllocator->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

	// Indicate a state transition on the resource usage.
	auto resourceBarrierTransitionRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &resourceBarrierTransitionRenderTarget);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	commandList->RSSetViewports(1, &screenViewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	const auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();

	const auto currentBackBufferView = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_dx12manager->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		currentBackBufferIndex,
		m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	const auto depthStencilView = m_dx12manager->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	// Clear the back buffer and depth buffer.
	commandList->ClearRenderTargetView(currentBackBufferView, Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilView,
	                                   D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
	                                   0, nullptr);

	// Specify the buffers we are going to render to.
	commandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilView);

	auto resourceBarrierTransitionPresent = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &resourceBarrierTransitionPresent);

	// Done recording commands.
	ThrowIfFailed(commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(swapChain->Present(0, 0));
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	m_dx12manager->FlushCommandQueue();
}
