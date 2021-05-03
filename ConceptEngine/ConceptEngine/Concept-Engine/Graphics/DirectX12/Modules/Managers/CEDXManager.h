#pragma once

#include "../../../Main/Managers/CEGraphicsManager.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Managers {
	class CEDXManager final : public Main::Managers::CEGraphicsManager {
	public:
		CEDXManager();
		~CEDXManager();

		void Create() override;
		void Destroy() override;
	};
}
