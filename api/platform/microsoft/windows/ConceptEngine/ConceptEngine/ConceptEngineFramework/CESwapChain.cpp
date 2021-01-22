#include "CESwapChain.h"


#include "CEAdapter.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEHelper.h"
#include "CEResourceStateTracker.h"
#include "CETexture.h"
using namespace Concept::GraphicsEngine::Direct3D;

void CESwapChain::SetFullScreen(bool fullscreen) {
	if (m_fullscreen != fullscreen) {
		m_fullscreen = fullscreen;
	}
}

void CESwapChain::WaitForSwapChain() {
	/*
	 * Wait 1 second for swap chain
	 */
	DWORD result = ::WaitForSingleObjectEx(m_handleFrameLatencyWaitableObject, 1000, TRUE);
}

void CESwapChain::Resize(uint32_t width, uint32_t height) {
	if (m_width != width || m_height != height) {
		m_width = std::max(1u, width);
		m_height = std::max(1u, height);

		m_device.Flush();

		/*
		 * Release all references to back buffer textures.
		 */
		m_renderTarget.Reset();

		for (UINT i = 0; i < BufferCount; ++i) {
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
}

CESwapChain::CESwapChain(CEDevice& device, HWND hWnd, DXGI_FORMAT renderTargetFormat):
	m_device(device),
	m_commandQueue(device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	m_hWnd(hWnd),
	m_fenceValues{0},
	m_width(0u),
	m_height(0u),
	m_renderTargetFormat(renderTargetFormat),
	m_vSync(true),
	m_tearingSupported(false),
	m_fullscreen(false) {
	/*
	 * Only works for valid window handle
	 */
	assert(hWnd);

	/*
	 * Query direct command queue from device.
	 * Required to create swap chain
	 */
	auto d3d12CommandQueue = m_commandQueue.GetD3D12CommandQueue();
	/*
	 * Query factory from adapter that was used to create device
	 */
	auto adapter = m_device.GetAdapter();
	auto dxgiAdapter = adapter->GetDXGIAdapter();

	/*
	 * Get factory that was used to create adapter
	 */
	wrl::ComPtr<IDXGIFactory> dxgiFactory;
	wrl::ComPtr<IDXGIFactory5> dxgiFactory5;
	ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
	/*
	 * Change DXGIFactory to 5 to use check feature support
	 */
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory5));

	/*
	 * Check for tearing support
	 */
	BOOL allowTearing = FALSE;
	if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL)))) {
		m_tearingSupported = (allowTearing == TRUE);
	}

	/*
	 * Query window client width and height;
	 */
	RECT windowRect;
	::GetClientRect(hWnd, &windowRect);
}

CESwapChain::~CESwapChain() {
}

void CESwapChain::UpdateRenderTargetViews() {
	for (UINT i = 0; i < BufferCount; ++i) {
		wrl::ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
		CEResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);
		m_backBufferTextures[i] = m_device.CreateTexture(backBuffer);

		/*
		 * Set names for back buffer textures
		 */
		m_backBufferTextures[i]->SetName(L"BackBuffer[" + std::to_wstring(i) + L"]");
	}
}
