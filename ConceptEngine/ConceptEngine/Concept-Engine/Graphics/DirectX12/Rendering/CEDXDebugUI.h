#pragma once
#include "../../Main/Rendering/CEDebugUI.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {
	class CEDXDebugUI final : public Main::Rendering::CEDebugUI {
	public:
		CEDXDebugUI();
		~CEDXDebugUI() override;
		bool Create() override;
		void Release() override;
		void DrawDebugString(const std::string& debugString) override;
		void OnKeyPressed(const Core::Common::CEKeyPressedEvent& Event) override;
		void OnKeyReleased(const Core::Common::CEKeyReleasedEvent& Event) override;
		void OnKeyTyped(const Core::Common::CEKeyTypedEvent& Event) override;
		void OnMousePressed(const Core::Common::CEMousePressedEvent& Event) override;
		void OnMouseReleased(const Core::Common::CEMouseReleasedEvent& Event) override;
		void OnMouseScrolled(const Core::Common::CEMouseScrolledEvent& Event) override;
		void Render(Main::Rendering::CECommandList& commandList) override;
	};
}
