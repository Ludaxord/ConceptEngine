#pragma once
#include "CEResource.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEViewport : public CEResource {
	public:
		CEViewport(CEFormat format, uint32 width, uint32 height) : CEResource(), Format(format), Width(width),
		                                                           Height(height) {

		}

		~CEViewport() = default;

		virtual bool Resize(uint32 width, uint32 uint32) = 0;
		virtual bool Present(bool verticalSync) = 0;

		virtual CERenderTargetView* GetRenderTargetView() const = 0;
		virtual CETexture2D* GetBackBuffer() const = 0;

		uint32 GetWidth() const {
			return Width;
		}

		uint32 GetHeight() const {
			return Height;
		}

		CEFormat GetColorFormat() const {
			return Format;
		}

	protected:
		uint32 Width;
		uint32 Height;
		CEFormat Format;
	};
}
