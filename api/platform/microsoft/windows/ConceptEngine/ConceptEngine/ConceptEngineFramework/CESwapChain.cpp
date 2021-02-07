#include "CESwapChain.h"


#include "CEAdapter.h"
#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEHelper.h"
#include "CEResourceStateTracker.h"
#include "CETexture.h"
using namespace Concept::GraphicsEngine::Direct3D;

CESwapChain::CESwapChain(CEDevice& device, HWND hWnd, DXGI_FORMAT renderTargetFormat)
	: m_device(device)
	  , m_commandQueue(device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT))
	  , m_hWnd(hWnd)
	  , m_fenceValues{0}
	  , m_width(0u)
	  , m_height(0u)
	  , m_renderTargetFormat(renderTargetFormat)
	  , m_vSync(true)
	  , m_tearingSupported(false)
	  , m_fullscreen(false) {
	assert(hWnd); // Must be a valid window handle!

	// Query the direct command queue from the device.
	// This is required to create the swapchain.
	auto d3d12CommandQueue = m_commandQueue.GetD3D12CommandQueue();

	// Query the factory from the adapter that was used to create the device.
	auto adapter = m_device.GetAdapter();
	auto dxgiAdapter = adapter->GetDXGIAdapter();

	// Get the factory that was used to create the adapter.
	wrl::ComPtr<IDXGIFactory> dxgiFactory;
	wrl::ComPtr<IDXGIFactory5> dxgiFactory5;
	ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
	// Now get the DXGIFactory5 so I can use the IDXGIFactory5::CheckFeatureSupport method.
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory5));

	// Check for tearing support.
	BOOL allowTearing = FALSE;
	if (SUCCEEDED(
		dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL)))) {
		m_tearingSupported = (allowTearing == TRUE);
	}

	// Query the windows client width and height.
	RECT windowRect;
	::GetClientRect(hWnd, &windowRect);

	m_width = windowRect.right - windowRect.left;
	m_height = windowRect.bottom - windowRect.top;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = m_renderTargetFormat;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = {1, 0};
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	//Test version create full screen swap chain descriptor
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {0};
	fsSwapChainDesc.Windowed = TRUE;

	//DXGI does not allo creating swap chain targeting window which has fullscreen styles(no border + topmost).
	//Temporarily remove topmost property for creating swapchain
	bool prevIsFullScreen = IsFullScreen();
	if (prevIsFullScreen) {
		CEScreen::SetWindowZOrderToTopMost(hWnd, false);
	}

	// Now create the swap chain.
	wrl::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
	ThrowIfFailed(dxgiFactory5->CreateSwapChainForHwnd(d3d12CommandQueue.Get(), m_hWnd, &swapChainDesc, &fsSwapChainDesc,
	                                                   nullptr, &dxgiSwapChain1));

	if (prevIsFullScreen) {
		CEScreen::SetWindowZOrderToTopMost(hWnd, true);
	}
	// Cast to swapchain4
	ThrowIfFailed(dxgiSwapChain1.As(&m_dxgiSwapChain));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(dxgiFactory5->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	// Initialize the current back buffer index.
	m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// Set maximum frame latency to reduce input latency.
	m_dxgiSwapChain->SetMaximumFrameLatency(BufferCount - 1);
	// Get the SwapChain's waitable object.
	m_handleFrameLatencyWaitableObject = m_dxgiSwapChain->GetFrameLatencyWaitableObject();

	UpdateRenderTargetViews();
}

CESwapChain::~CESwapChain() {
	// Close the SwapChain's waitable event handle?? I don't think that's necessary!?
	//::CloseHandle( m_hFrameLatencyWaitableObject );
}

void CESwapChain::SetFullScreen(bool fullscreen) {
	if (m_fullscreen != fullscreen) {
		m_fullscreen = fullscreen;
		// TODO:
		//        m_dxgiSwapChain->SetFullscreenState()
	}
}

void CESwapChain::WaitForSwapChain() {
	DWORD result = ::WaitForSingleObjectEx(m_handleFrameLatencyWaitableObject, 1000,
	                                       TRUE); // Wait for 1 second (should never have to wait that long...)
}

void CESwapChain::Resize(uint32_t width, uint32_t height) {
	if (m_width != width || m_height != height) {
		m_width = std::max(1u, width);
		m_height = std::max(1u, height);

		m_device.Flush();

		// Release all references to back buffer textures.
		m_renderTarget.Reset();
		for (UINT i = 0; i < BufferCount; ++i) {
			//ResourceStateTracker::RemoveGlobalResourceState( m_BackBufferTextures[i]->GetD3D12Resource().Get(), true );
			m_backBufferTextures[i].reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(BufferCount, m_width, m_height, swapChainDesc.BufferDesc.Format,
		                                             swapChainDesc.Flags));

		m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews();
	}
}

const CERenderTarget& CESwapChain::GetRenderTarget() const {
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, m_backBufferTextures[m_currentBackBufferIndex]);
	return m_renderTarget;
}

UINT CESwapChain::Present(const std::shared_ptr<CETexture>& texture) {
	auto commandList = m_commandQueue.GetCommandList();

	auto backBuffer = m_backBufferTextures[m_currentBackBufferIndex];

	if (texture) {
		if (texture->GetD3D12ResourceDesc().SampleDesc.Count > 1) {
			commandList->ResolveSubResource(backBuffer, texture);
		}
		else {
			commandList->CopyResource(backBuffer, texture);
		}
	}

	commandList->TransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
	m_commandQueue.ExecuteCommandList(commandList);

	UINT syncInterval = m_vSync ? 1 : 0;
	UINT presentFlags = m_tearingSupported && !m_fullscreen && !m_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));

	m_fenceValues[m_currentBackBufferIndex] = m_commandQueue.Signal();

	m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	auto fenceValue = m_fenceValues[m_currentBackBufferIndex];
	m_commandQueue.WaitForFenceValue(fenceValue);

	m_device.ReleaseStaleDescriptors();

	return m_currentBackBufferIndex;
}

void CESwapChain::UpdateRenderTargetViews() {
	for (UINT i = 0; i < BufferCount; ++i) {
		wrl::ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		CEResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);

		m_backBufferTextures[i] = m_device.CreateTexture(backBuffer);

		// Set the names for the backbuffer textures.
		// Useful for debugging.
		m_backBufferTextures[i]->SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
	}
}
