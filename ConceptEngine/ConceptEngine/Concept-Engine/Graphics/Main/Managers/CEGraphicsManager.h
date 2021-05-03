#pragma once

#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Managers {
	class CEGraphicsManager : public Core::Common::CEManager {
	public:
		CEGraphicsManager() = default;
		virtual ~CEGraphicsManager() = default;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
	};
}
