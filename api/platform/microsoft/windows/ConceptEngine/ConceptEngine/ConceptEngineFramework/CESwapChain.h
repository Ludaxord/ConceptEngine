#pragma once
#include <cstdint>
#include <dxgi1_5.h>
#include <dxgiformat.h>
#include <memory>
#include <wrl.h>

#include "CERenderTarget.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CECommandQueue;
	class CEDevice;
	class CETexture;
	namespace wrl = Microsoft::WRL;

	class CESwapChain {
	public:
		/*
		 * Numbers of SwapChain back buffers;
		 */
		static const UINT BufferCount = 3;

		/*
		 * Check to see if Swap Chain is in full screen exclusive mode
		 */
		bool IsFullScreen() const {
			return m_fullscreen;
		}

		/*
		 * Set swap chain to full screen exclusive mode (true) or windowed mode (false)
		 */
		void SetFullScreen(bool fullscreen);

		/*
		 * Toggle fullscreen exclusive mode
		 */
		void ToggleFullScreen() {
			SetFullScreen(!m_fullscreen);
		}

		/*
		 * Set VSync mode
		 */
		void SetVSync(bool vSync) {
			m_vSync = vSync;
		}

		bool GetVSync() const {
			return m_vSync;
		}

		void ToggleVSync() {
			SetVSync(!m_vSync);
		}

		/*
		 * Check if Tearing is supported
		 */
		bool IsTearingSupported() const {
			return m_tearingSupported;
		}

		/*
		 * Block current thread until swap chain has finished presenting;
		 * Doing that at the beginning of update loop can improve input latency.
		 */
		void WaitForSwapChain();

		/*
		 * resize swap chain back buffers,
		 * should be called if window is resized
		 */
		void Resize(uint32_t width, uint32_t height);

		/*
		 * Get Render target of window,
		 * should be called every frame since the color attachment point changes depending on
		 * window current back buffer
		 */
		const CERenderTarget& GetRenderTarget() const;

		/*
		 * Present Swap Chain back buffer to screen
		 * @param [texture] texture to copy to swap chain back buffer before presenting.
		 * By default, passed empty texture, in this case no copy will be performed.
		 * Use CESwapChain::RenderTarget to get render target for window color buffer
		 * @returns current Back Buffer index after present;
		 */
		UINT Present(const std::shared_ptr<CETexture>& texture = nullptr);

		/*
		 * Get format which Back buffer was created;
		 */
		DXGI_FORMAT GetRenderTargetFormat() const {
			return m_renderTargetFormat;
		}

		wrl::ComPtr<IDXGISwapChain4> GetDXGISwapChain() const {
			return m_dxgiSwapChain;
		}

	protected:
		CESwapChain(CEDevice& device, HWND hWnd,
		                   DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
		virtual ~CESwapChain();
		/*
		 * Update swap chain RTV.
		 */
		void UpdateRenderTargetViews();

	private:
		/*
		 *device used to create swap chain
		*/
		CEDevice& m_device;

		/*
		 * Command Queue that is used to create swap chain
		 * Comamnd Queue will be signaled right after present
		 * To make sure that Swap Chain back buffer are not in use
		 * before next frame is allowed to be rendered on screen;
		*/
		CECommandQueue& m_commandQueue;
		wrl::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
		std::shared_ptr<CETexture> m_backBufferTextures[BufferCount];
		mutable CERenderTarget m_renderTarget;

		/*
		 * Current back buffer index of swap chain.
		 */
		UINT m_currentBackBufferIndex;

		/*
		 * fence values to wait before leaving Present method
		 */
		UINT64 m_fenceValues[BufferCount];

		/*
		 * Handle to waitable objects. Used to wait for Swap Chain before presentation
		 */
		HANDLE m_handleFrameLatencyWaitableObject;

		/*
		 * Window handle that is associates with swap chain
		 */
		HWND m_hWnd;

		/*
		 * Swap chain width/height
		 */
		uint32_t m_width;
		uint32_t m_height;

		/*
		 * Format of back buffer
		 */
		DXGI_FORMAT m_renderTargetFormat;

		/*
		 * Set state if application is in full screen mode or windowed mode
		 */
		bool m_fullscreen;

		/*
		 * should presentation be synchronized with vertical refresh rate of the screen
		 * Remove screen tearing by setting variable to True
		 */
		bool m_vSync;

		/*
		 * Check if GPU support tearing
		 */
		bool m_tearingSupported;
	};
}
