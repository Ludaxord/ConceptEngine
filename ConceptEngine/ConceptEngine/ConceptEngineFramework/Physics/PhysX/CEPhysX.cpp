#include "CEPhysX.h"

using namespace ConceptEngineFramework::Physics::PhysX;

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

void CEPhysX::InitPhysics() {
}

void CEPhysX::CreateScene() {
}

void CEPhysX::CleanUpScene() {
}

void CEPhysX::CreateStaticObject(void* pDesc) {
}

void CEPhysX::CreateDynamicObject(void* pDesc) {
}

void CEPhysX::AddCharacterController(void* pDesc) {
}

void CEPhysX::RemoveCharacterController(const std::string& name) {
}

void CEPhysX::MoveCharacterController(const std::string& name, float& dispX, float& dispY, float& dispZ, float minDist,
	float elapsedTime) {
}

void CEPhysX::Update(float delta) {
}

void CEPhysX::CleanUpPhysics() {
}

void CEPhysX::AddForce(std::string name, float x, float y, float z) {
}
