#include "CERenderTarget.h"

#include "CETexture.h"
using namespace Concept::GraphicsEngine::Direct3D;

CERenderTarget::CERenderTarget(): m_textures(AttachmentPoint::NumAttachmentPoints), m_size(0, 0) {
}

void CERenderTarget::AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<CETexture> texture) {
	m_textures[attachmentPoint] = texture;
	if (texture && texture->GetD3D12Resource()) {
		auto desc = texture->GetD3D12ResourceDesc();
		m_size.x = static_cast<uint32_t>(desc.Width);
		m_size.y = static_cast<uint32_t>(desc.Height);
	}
}

std::shared_ptr<CETexture> CERenderTarget::GetTexture(AttachmentPoint attachmentPoint) const {
	return m_textures[attachmentPoint];
}

void CERenderTarget::Resize(::DirectX::XMUINT2 size) {
	m_size = size;
	for (auto texture : m_textures) {
		if (texture) {
			texture->Resize(m_size.x, m_size.y);
		}
	}
}

void CERenderTarget::Resize(uint32_t width, uint32_t height) {
	Resize(DirectX::XMUINT2(width, height));
}

::DirectX::XMUINT2 CERenderTarget::GetSize() const {
	return m_size;
}

uint32_t CERenderTarget::GetWidth() const {
	return m_size.x;
}

uint32_t CERenderTarget::GetHeight() const {
	return m_size.y;
}

D3D12_VIEWPORT CERenderTarget::GetViewPort(::DirectX::XMFLOAT2 scale, ::DirectX::XMFLOAT2 bias, float minDepth,
                                           float maxDepth) const {
	UINT64 width = 0;
	UINT height = 0;

	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i) {
		auto texture = m_textures[i];
		if (texture) {
			auto desc = texture->GetD3D12ResourceDesc();
			width = std::max(width, desc.Width);
			height = std::max(height, desc.Height);
		}
	}

	D3D12_VIEWPORT viewport = {
		(width * bias.x), //TopLeftX
		(height * bias.y), //TopLeftY
		(width * scale.x), //Width
		(height * scale.y), //Height
		minDepth, //MinDepth
		maxDepth //MaxDepth
	};

	return viewport;
}

/*
 * Get list of textures attached to render target.
 * This method is primarily used by commandList
 * when binding render target to output merger stage of rendering pipeline
 */
const std::vector<std::shared_ptr<CETexture>>& CERenderTarget::GetTextures() const {
	return m_textures;
}

D3D12_RT_FORMAT_ARRAY CERenderTarget::GetRenderTargetFormats() const {
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i) {
		auto texture = m_textures[i];
		if (texture) {
			rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture->GetD3D12ResourceDesc().Format;
		}
	}
	return rtvFormats;
}

DXGI_FORMAT CERenderTarget::GetDepthStencilFormat() const {
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
	auto depthStencilTexture = m_textures[AttachmentPoint::DepthStencil];
	if (depthStencilTexture) {
		dsvFormat = depthStencilTexture->GetD3D12ResourceDesc().Format;
	}

	return dsvFormat;
}

DXGI_SAMPLE_DESC CERenderTarget::GetSampleDesc() const {
	DXGI_SAMPLE_DESC sampleDesc = {1, 0};
	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i) {
		auto texture = m_textures[i];
		if (texture) {
			sampleDesc = texture->GetD3D12ResourceDesc().SampleDesc;
			break;
		}
	}
	return sampleDesc;
}
