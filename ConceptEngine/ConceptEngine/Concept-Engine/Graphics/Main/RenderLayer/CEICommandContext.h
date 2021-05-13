#pragma once

#include "../../../Core/Common/CERefCountedObject.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEICommandContext : public Core::Common::CERefCountedObject {
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void BeginTimeStamp() = 0;
		virtual void EndTimeStamp() = 0;
		virtual void ClearRenderTargetView() = 0;
		virtual void ClearDepthStencilView() = 0;
		virtual void ClearUnorderedAccessViewFloat() = 0;
		virtual void SetShadingRate() = 0;
		virtual void SetShadingRateImage() = 0;
		virtual void SetViewport() = 0;
		virtual void SetScissorRect() = 0;
		virtual void SetBlendFactor() = 0;
	};
}
