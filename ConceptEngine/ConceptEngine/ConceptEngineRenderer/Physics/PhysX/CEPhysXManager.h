#pragma once

struct CEPhysicsLayer {
	uint32 LayerID;
	std::string Name;
	uint32 BitValue;
	int32 CollidesWith = 0;

	bool IsValid() const {
		return !Name.empty() && BitValue > 0;
	}
};

class CEPhysXManager {
public:
	bool Create(const std::string& Name = "Default");

	uint32 AddLayer(const std::string& Name, bool SetCollisions = true);
	void RemoveLayer(uint32 LayerID);

	void SetLayerCollision(uint32 MainLayerID, uint32 SecondLayerID, bool ShouldCollide);
	CEArray<CEPhysicsLayer> GetLayerCollisions(uint32 LayerID);

	const CEArray<CEPhysicsLayer>& GetLayers() {
		return Layers;
	}

	const CEArray<std::string>& GetLayerNames() {
		return LayerNames;
	}

	CEPhysicsLayer& GetLayer(uint32 LayerID);
	CEPhysicsLayer& GetLayer(const std::string& LayerName);

	uint32 GetLayerCount() {
		return uint32(Layers.Size());
	}

	bool ShouldCollide(uint32 Layer1, uint32 Layer2);
	bool IsLayerValid(uint32 LayerID);

private:
	uint32 GetNextLayerID();

private:
	CEArray<CEPhysicsLayer> Layers;
	CEArray<std::string> LayerNames;
	CEPhysicsLayer NullLayer;
};
