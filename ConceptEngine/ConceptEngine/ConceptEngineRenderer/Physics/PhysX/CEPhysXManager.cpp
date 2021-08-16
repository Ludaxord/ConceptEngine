#include "CEPhysXManager.h"

template <typename T, typename ConditionFunction>
bool RemoveIfExists(CEArray<T>& Array, ConditionFunction Condition) {
	for (typename CEArray<T>::Iterator It = Array.Begin(); It != Array.End(); ++It) {
		if (Condition(*It)) {
			Array.Erase(It);
			return true;
		}
	}

	return false;
}

CEPhysXManager::CEPhysXManager() = default;

bool CEPhysXManager::Create(const std::string& Name) {
	if (AddLayer(Name) == -1) {
		CE_LOG_ERROR("[CEPhysXManager]: Failed to AddLayer by Name: " + Name)
		return false;
	}

	return true;
}

uint32 CEPhysXManager::AddLayer(const std::string& Name, bool SetCollisions) {
	uint32 LayerID = GetNextLayerID();
	CE_LOG_DEBUG("[CEPhysXManager]: Add Layer ID: " + std::to_string(LayerID));
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
		return Layer.LayerID == LayerID;
	});
}

void CEPhysXManager::SetLayerCollision(uint32 MainLayerID, uint32 SecondLayerID, bool ShouldCollide) {
	if (CEPhysXManager::ShouldCollide(MainLayerID, SecondLayerID) && ShouldCollide) {
		return;
	}

	CEPhysicsLayer& MainLayerInfo = GetLayer(MainLayerID);
	CEPhysicsLayer& SecondLayerInfo = GetLayer(SecondLayerID);

	if (ShouldCollide) {
		MainLayerInfo.CollidesWith |= SecondLayerInfo.BitValue;
		SecondLayerInfo.CollidesWith |= MainLayerInfo.BitValue;
	}
	else {
		MainLayerInfo.CollidesWith &= ~SecondLayerInfo.BitValue;
		SecondLayerInfo.CollidesWith &= ~MainLayerInfo.BitValue;
	}
}

CEArray<CEPhysicsLayer> CEPhysXManager::GetLayerCollisions(uint32 LayerID) {
	const CEPhysicsLayer& Layer = GetLayer(LayerID);

	CEArray<CEPhysicsLayer> CollisionsLayers;
	for (const CEPhysicsLayer& MainLayer : Layers) {
		if (MainLayer.LayerID == LayerID) {
			continue;
		}

		if (Layer.CollidesWith & MainLayer.BitValue) {
			CollisionsLayers.PushBack(MainLayer);
		}
	}

	return CollisionsLayers;

}

CEPhysicsLayer& CEPhysXManager::GetLayer(uint32 LayerID) {
	return LayerID >= Layers.Size() ? NullLayer : Layers[LayerID];
}

CEPhysicsLayer& CEPhysXManager::GetLayer(const std::string& LayerName) {
	for (CEPhysicsLayer& Layer : Layers) {
		if (Layer.Name == LayerName) {
			return Layer;
		}
	}

	return NullLayer;
}

bool CEPhysXManager::ShouldCollide(uint32 Layer1, uint32 Layer2) {
	return GetLayer(Layer1).CollidesWith & GetLayer(Layer2).BitValue;
}

bool CEPhysXManager::IsLayerValid(uint32 LayerID) {
	const CEPhysicsLayer& Layer = GetLayer(LayerID);

	return Layer.LayerID != NullLayer.LayerID && Layer.IsValid();
}

uint32 CEPhysXManager::GetNextLayerID() {
	int32 LastID = -1;

	for (const CEPhysicsLayer& Layer : Layers) {
		if (LastID != -1) {
			if (Layer.LayerID != LastID + 1) {
				return LastID + 1;
			}
		}

		LastID = Layer.LayerID;
	}

	return (uint32)Layers.Size();
}
