#pragma once

#include "../../../Main/Managers/CETextureManager.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Managers {
	class CEDXTextureManager final : public Main::Managers::CETextureManager {
	public:
		CEDXTextureManager();
		~CEDXTextureManager() override;
	protected:
	private:
	};

}
