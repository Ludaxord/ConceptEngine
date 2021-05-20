#pragma once
#include "../../Main/Rendering/CEDebugUI.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {
	class CEDXDebugUI final : public Main::Rendering::CEDebugUI {
	public:
		CEDXDebugUI();
		~CEDXDebugUI() override;
		bool Create() override;
	};
}
