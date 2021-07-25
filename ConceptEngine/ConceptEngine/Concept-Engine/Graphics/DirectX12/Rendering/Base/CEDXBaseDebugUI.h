#pragma once
#include "../../../Main/Rendering/CEDebugUI.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering::Base {
	class CEDXBaseDebugUI final : public Main::Rendering::CEDebugUI {
	public:
		CEDXBaseDebugUI();
		~CEDXBaseDebugUI() override;
		bool Create() override;
		void Release() override;
		void DrawDebugString(const std::string& debugString) override;
		void OnKeyPressed(const Core::Common::CEKeyPressedEvent& Event) override;
		void OnKeyReleased(const Core::Common::CEKeyReleasedEvent& Event) override;
		void OnKeyTyped(const Core::Common::CEKeyTypedEvent& Event) override;
		void OnMousePressed(const Core::Common::CEMousePressedEvent& Event) override;
		void OnMouseReleased(const Core::Common::CEMouseReleasedEvent& Event) override;
		void OnMouseScrolled(const Core::Common::CEMouseScrolledEvent& Event) override;

		void Render(Main::RenderLayer::CECommandList& commandList) override;
		
		static void OnKeyEventPressed(const Core::Common::CEKeyPressedEvent& Event);
		static void OnKeyEventReleased(const Core::Common::CEKeyReleasedEvent& Event);
		static void OnKeyEventTyped(const Core::Common::CEKeyTypedEvent& Event) ;
		static void OnMouseEventPressed(const Core::Common::CEMousePressedEvent& Event);
		static void OnMouseEventReleased(const Core::Common::CEMouseReleasedEvent& Event) ;
		static void OnMouseEventScrolled(const Core::Common::CEMouseScrolledEvent& Event) ;

		static ImGuiContext* GetCurrentContext();
		void DrawUI(UIDrawFunc DrawFunc) override;
	};
}
