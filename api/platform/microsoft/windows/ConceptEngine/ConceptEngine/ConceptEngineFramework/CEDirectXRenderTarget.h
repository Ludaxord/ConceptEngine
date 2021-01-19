#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXTexture;
	namespace wrl = Microsoft::WRL;

	enum AttachmentPoint {
		Color0,
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Color6,
		Color7,
		DepthStencil,
		NumAttachmentPoints
	};

	class CEDirectXRenderTarget {
	public:
		/*
		 * Create enpty render target;
		 */
		CEDirectXRenderTarget();

		CEDirectXRenderTarget(const CEDirectXRenderTarget& copy) = default;
		CEDirectXRenderTarget(CEDirectXRenderTarget&& copy) = default;

		/*
		 * Attach texture to given attachment point.
		 *
		 * @param [attachmentPoint], point to attach texture to,
		 * @param [texture] Optional texture to bind render target, Pass nullptr to clear texture;
		 */
		void AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<CEDirectXTexture> texture);
		std::shared_ptr<CEDirectXTexture> GetTexture(AttachmentPoint attachmentPoint) const;

		/*
		 * Resize all textures associated with render target;
		 */
		void Resize(::DirectX::XMUINT2 size);
		void Resize(uint32_t width, uint32_t height);
		::DirectX::XMUINT2 GetSize() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		/*
		 * Get viewPort for render target,
		 * scale and bias parameters can be used to specify a split-screen,
		 * viewport (bias parameter is normalized in range [0...1]).
		 * By default, fullscreen viewport is returned.
		 */
		D3D12_VIEWPORT GetViewPort(::DirectX::XMFLOAT2 scale = {1.0f, 1.0f},
		                           ::DirectX::XMFLOAT2 bias = {0.0f, 0.0f},
		                           float minDepth = 0.0f, float maxDepth = 1.0f
		) const;

		/*
		 * Get list of textures attached to render target;
		 * Method is primarily used by CommandList when binding
		 * render target to output merger stage of rendering pipeline
		 */
		const std::vector<std::shared_ptr<CEDirectXTexture>>& GetTextures() const;

		/*
		 * Get render target formats of textures currently
		 * attached to this render target object.
		 * is should be used to configure pipeline state object.
		 */
		D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

		/*
		 * Get format of attached depth stencil buffer
		 */
		DXGI_FORMAT GetDepthStencilFormat() const;

		/*
		 * Get sample description of render target;
		 */
		DXGI_SAMPLE_DESC GetSampleDesc() const;

		/*
		 * Reset all textures
		 */
		void Reset() {
			m_textures = RenderTargetList(AttachmentPoint::NumAttachmentPoints);
		}

	private:
		using RenderTargetList = std::vector<std::shared_ptr<CEDirectXTexture>>;
		RenderTargetList m_textures;
		::DirectX::XMUINT2 m_size;
	};

}
