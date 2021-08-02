#pragma once
#include "D3D12/CEDX12GraphicsManager.h"
#include "RenderLayer/CEGraphicsManager.h"
#include "Graphics/Generic/Managers/CETextureManager.h"
#include "Graphics/Generic/Managers/CEMeshManager.h"

enum class ManagerType {
	Graphics = 0,
	Texture = 1,
	Mesh = 2
};

inline CEManager* GetManager(ManagerType Type) {
	switch (Type) {
	case ManagerType::Graphics:
		return GraphicsManager;
	case ManagerType::Texture:
		return TextureManager;
	case ManagerType::Mesh:
		return MeshManager;
	default:
		return nullptr;
	}
}


template <typename T>
inline T* CastManager(ManagerType Type) {
	return dynamic_cast<T*>(GetManager(Type));
}

inline CEGraphicsManager* CastGraphicsManager() {
	return CastManager<CEGraphicsManager>(ManagerType::Graphics);
}

#if PLATFORM_WINDOWS
inline CEDX12GraphicsManager* CastDX12GraphicsManager() {
	return CastManager<CEDX12GraphicsManager>(ManagerType::Graphics);
}
#endif
