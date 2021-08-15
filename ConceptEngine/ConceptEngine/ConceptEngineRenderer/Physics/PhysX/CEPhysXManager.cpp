#include "CEPhysXManager.h"

template <typename T, typename ConditionFunction>
bool RemoveIfExists(CEArray<T>& Array, ConditionFunction Condition) {
	for (CEArray<T>::Iterator It = Array.Begin(); It != Array.End(); ++It) {
		if (Condition(*It)) {
			Array.Erase(It);
			return true;
		}
	}

	return false;
}

bool CEPhysXManager::Create(const std::string& Name) {
	if (AddLayer(Name) != -1) {
		CE_LOG_ERROR("[CEPhysXManager]: Failed to AddLayer by Name: " + Name)
		return false;
	}

	return true;
}

uint32 CEPhysXManager::AddLayer(const std::string& Name, bool SetCollisions) {
	uint32 LayerID = GetNextLayerID();
	CEPhysicsLayer Layer = {LayerID, Name, 1 << LayerID, 1 << LayerID};
	Layers.Insert(Layers.Begin() + LayerID, Layer);
	LayerNames.Insert(LayerNames.begin() + LayerID, Name);

	if (SetCollisions) {
		for (CEPhysicsLayer& SecondLayer : Layers) {
			SetLayerCollision(Layer.LayerID, SecondLayer.LayerID, true);
		}
	}

	return Layer.LayerID;
}

void CEPhysXManager::RemoveLayer(uint32 LayerID) {
	CEPhysicsLayer& LayerInfo = GetLayer(LayerID);
	for (CEPhysicsLayer& SecondLayer : Layers) {
		if (SecondLayer.LayerID == LayerID) {
			continue;
		}

		if (SecondLayer.CollidesWith & LayerInfo.BitValue) {
			SecondLayer.CollidesWith &= ~LayerInfo.BitValue;
		}
	}

	RemoveIfExists<std::string>(LayerNames, [&](const std::string& Name) {
		return Name == LayerInfo.Name;
	});
	RemoveIfExists<CEPhysicsLayer>(Layers, [&](const CEPhysicsLayer& Layer) {
		return Layer.LayerID== LayerID;
	});
}

void CEPhysXManager::SetLayerCollision(uint32 MainLayerID, uint32 SecondLayerID, bool ShouldCollide) {
}

CEArray<CEPhysicsLayer> CEPhysXManager::GetLayerCollisions(uint32 LayerID) {
}

CEPhysicsLayer& CEPhysXManager::GetLayer(uint32 LayerID) {
}

CEPhysicsLayer& CEPhysXManager::GetLayer(const std::string& LayerName) {
}

bool CEPhysXManager::ShouldCollide(uint32 Layer1, uint32 Layer2) {
}

bool CEPhysXManager::IsLayerValid(uint32 LayerID) {
}

uint32 CEPhysXManager::GetNextLayerID() {
}
