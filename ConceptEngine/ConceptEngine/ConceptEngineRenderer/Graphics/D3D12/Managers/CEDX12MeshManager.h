#pragma once
#include "Graphics/Generic/Managers/CEMeshManager.h"

class CEDX12MeshManager : public CEMeshManager {
public:
	CEDX12MeshManager();
	~CEDX12MeshManager();
	bool Create() override;
	void Release() override;
};
