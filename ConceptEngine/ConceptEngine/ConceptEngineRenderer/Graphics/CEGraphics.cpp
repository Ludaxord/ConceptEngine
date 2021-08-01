#include "CEGraphics.h"

CEGraphics::CEGraphics(): GraphicsManager(nullptr), TextureManager(nullptr), MeshManager(nullptr) {
}

bool CEGraphics::Create() {
	return false;
}

bool CEGraphics::CreateManagers() {
	return false;
}

bool CEGraphics::Release() {
	return false;
}

CEDebugUI CEGraphics::GetDebugUI() {
	return {};
}

CEManager* CEGraphics::GetManager(ManagerType Type) const {
	return nullptr;
}
