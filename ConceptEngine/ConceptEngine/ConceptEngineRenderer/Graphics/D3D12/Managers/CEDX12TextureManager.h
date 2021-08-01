#pragma once
#include "Graphics/Generic/Managers/CETextureManager.h"


class CEDX12TextureManager : public CETextureManager {
public:
	CEDX12TextureManager();
	~CEDX12TextureManager();
	bool Create() override;
	void Release() override;
};
