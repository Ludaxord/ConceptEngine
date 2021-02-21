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
	spdlog::warn("CEDX12InitDirect3DPlayground Update");
}

void CEDX12InitDirect3DPlayground::Render(const CETimer& gt) {
	spdlog::warn("CEDX12InitDirect3DPlayground Render");
	//Reuse memory associated with command recording
	//We can only reset when associated command lists have finished execution on GPU
	ThrowIfFailed(m_dx12manager->GetD3D12CommandAllocator()->Reset());

	//Command list can be reset after it has been added to command queue via ExecuteCommandList
	//Reusing command list reuses memory
	ThrowIfFailed(
		m_dx12manager->GetD3D12CommandList()->Reset(m_dx12manager->GetD3D12CommandAllocator().Get(), nullptr));

	//Indicate a state transition on resource usage
	auto rtvTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                          D3D12_RESOURCE_STATE_PRESENT,
	                                                          D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &rtvTransition);

	//Set viewport and scissor rect. This needs to be reset whenever command list is rest.
	auto viewPort = m_dx12manager->GetViewPort();
	auto scissorRect = m_dx12manager->GetScissorRect();

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &viewPort);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &scissorRect);

	//Clear back buffer and depth buffer.
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_dx12manager->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			m_dx12manager->GetCurrentBackBuffer(),
			m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)),
		DirectX::Colors::LightSteelBlue,
		0,
		nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(
		m_dx12manager->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr);

	auto rtvCurrentBackBufferView = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_dx12manager->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		m_dx12manager->GetCurrentBackBuffer(),
		m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	auto rtvDepthStencilView = m_dx12manager->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//Specify buffers we are going to render to.
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1,
	                                                         &rtvCurrentBackBufferView,
	                                                         true,
	                                                         &rtvDepthStencilView);

	//Indicate state transition on resource usage
	auto rtvPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                                 D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                                 D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &rtvPresentTransition);

	//Done recording commands
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());

	//Add command list to queue for execution
	ID3D12CommandList* commandLists[] = {m_dx12manager->GetD3D12CommandList().Get()};
	m_dx12manager->GetD3D12CommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

	//swap the back and front buffers
	const UINT syncInterval = m_dx12manager->GetVSync() ? 1 : 0;
	const UINT presentFlags = m_dx12manager->GetTearingSupport() &&
	                          !m_dx12manager->IsFullScreen() &&
	                          !m_dx12manager->GetVSync()
		                          ? DXGI_PRESENT_ALLOW_TEARING
		                          : 0;
	ThrowIfFailed(m_dx12manager->GetDXGISwapChain()->Present(syncInterval, presentFlags));

	auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	m_dx12manager->SetCurrentBackBufferIndex(
		currentBackBufferIndex + 1 % CEDX12Manager::GetBackBufferCount());

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.
	m_dx12manager->FlushCommandQueue();
}
