#pragma once
#include "../../../Main/Managers/CERendererManager.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Managers {
	class CEDXRendererManager final : public Main::Managers::CERendererManager {
	public:
		CEDXRendererManager();
		~CEDXRendererManager() override;
		bool Create() override;
	};
}
