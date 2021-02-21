#pragma once
#include <memory>

#include "../CEGraphicsManager.h"

namespace ConceptEngineFramework::Graphics::Vulkan {
	class CEVManager : public CEGraphicsManager {

	public:
		void Create() override;
		void Destroy() override;
		void Resize() override;
		bool Initialized() override;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEVManager>;

		CEVManager();
		~CEVManager() = default;

	};
}
