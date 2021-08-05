#pragma once
#include "../CEPhysics.h"


namespace ConceptEngine::Physics::PhysX {
	class CEPhysX : public CEPhysics {
	public:
		bool Create() override;
		bool CreateScene() override;


	};
}
