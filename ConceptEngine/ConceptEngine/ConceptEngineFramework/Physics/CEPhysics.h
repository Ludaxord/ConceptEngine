#pragma once
#include <combaseapi.h>
#include <string>

namespace ConceptEngineFramework::Physics {
	interface CEPhysics {
	public:
		virtual ~CEPhysics() = default;
		virtual void InitPhysics() = 0;
		virtual void CreateScene() = 0;
		virtual void CleanUpScene() = 0;
		virtual void CreateStaticObject(void* pDesc) = 0;
		virtual void CreateDynamicObject(void* pDesc) = 0;

		virtual void AddCharacterController(void* pDesc) = 0;
		virtual void RemoveCharacterController(const std::string& name) = 0;
		virtual void MoveCharacterController(const std::string& name,
		                                     float& dispX,
		                                     float& dispY,
		                                     float& dispZ,
		                                     float minDist,
		                                     float elapsedTime) = 0;
		virtual void Update(float delta) = 0;

		virtual void CleanUpPhysics() = 0;
	protected:
		virtual void AddForce(std::string name, float x, float y, float z) = 0;

	private:
	};
}
