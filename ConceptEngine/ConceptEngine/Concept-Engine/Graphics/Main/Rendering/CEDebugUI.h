#pragma once

#include "../../../Core/Common/CERef.h"

#include "../RenderLayer/CEResourceViews.h"

#include "../../../Core/Common/CEEvents.h"

#include <imgui.h>

namespace ConceptEngine::Graphics::Main::Rendering {
	struct ImGuiImage {
		ImGuiImage() = default;

		ImGuiImage(const Core::Common::CERef<RenderLayer::CEShaderResourceView>& imageView,
		           const Core::Common::CERef<RenderLayer::CETexture>& image, RenderLayer::CEResourceState before,
		           RenderLayer::CEResourceState after): ImageView(imageView), Image(image), BeforeState(before),
		                                                AfterState(after) {
		}

		Core::Common::CERef<RenderLayer::CEShaderResourceView> ImageView;
		Core::Common::CERef<RenderLayer::CETexture> Image;
		RenderLayer::CEResourceState BeforeState;
		RenderLayer::CEResourceState AfterState;
		bool AllowBlending = false;
	};

	class CEDebugUI {
	public:
		CEDebugUI();
		virtual ~CEDebugUI();

		typedef void (*UIDrawFunc)();

		virtual bool Create() = 0;
		virtual void Release() = 0;

		virtual void DrawUI(UIDrawFunc DrawFunc);
		virtual void DrawDebugString(const std::string& debugString) = 0;

		virtual void OnKeyPressed(const Core::Common::CEKeyPressedEvent& Event) = 0;
		virtual void OnKeyReleased(const Core::Common::CEKeyReleasedEvent& Event) = 0;
		virtual void OnKeyTyped(const Core::Common::CEKeyTypedEvent& Event) = 0;

		virtual void OnMousePressed(const Core::Common::CEMousePressedEvent& Event) = 0;
		virtual void OnMouseReleased(const Core::Common::CEMouseReleasedEvent& Event) = 0;
		virtual void OnMouseScrolled(const Core::Common::CEMouseScrolledEvent& Event) = 0;

		virtual void Render(class CECommandList& commandList) = 0;

		static ImGuiContext* GetCurrentContext();
	protected:
	private:
	};
}
