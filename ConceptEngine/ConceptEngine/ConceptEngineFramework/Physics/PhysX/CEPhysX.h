#pragma once

#include "../CEPhysics.h"

namespace ConceptEngineFramework::Physics::PhysX {
	class CEPhysX : public CEPhysics {
	public:
		CEPhysX();
		~CEPhysX();
		
		void InitPhysics() override;
		void CreateScene() override;
		void CleanUpScene() override;
		void CreateStaticObject(void* pDesc) override;
		void CreateDynamicObject(void* pDesc) override;
		void AddCharacterController(void* pDesc) override;
		void RemoveCharacterController(const std::string& name) override;
		void MoveCharacterController(const std::string& name, float& dispX, float& dispY, float& dispZ, float minDist,
			float elapsedTime) override;
		void Update(float delta) override;
		void CleanUpPhysics() override;
	protected:
		void AddForce(std::string name, float x, float y, float z) override;
	private:
	};
}
