#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::DirectX12 {
	class CEDirectX12 : public Main::CEGraphics {
	public:
		CEDirectX12();
		virtual ~CEDirectX12();
		
		void Create() override;
		void CreateManagers() override;
	};
}
