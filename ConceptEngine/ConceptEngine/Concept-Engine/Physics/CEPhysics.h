#pragma once

namespace ConceptEngine::Physics {
	class CEPhysics {
	public:
		virtual bool Create() = 0;
		virtual bool CreateScene() = 0;
	};
}
