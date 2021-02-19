#pragma once
#include "../CEGraphicsManager.h"

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager : public CEGraphicsManager {

	public:
		void Create() override;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEDX12Manager>;

		CEDX12Manager();
		~CEDX12Manager() = default;

	};
}
