#include "CETextureManager.h"

using namespace ConceptEngine::Graphics::Main::Managers;
using namespace ConceptEngine::Graphics::Main::RenderLayer;

CETextureManager::CETextureManager(): CEManager() {
}

CETextureManager::~CETextureManager() {
}

bool CETextureManager::Create() {
	return true;
}

CETexture2D* CETextureManager::LoadFromFile(const std::string& filePath, uint32 flags, RenderLayer::CEFormat format) {
	return nullptr;
}

CETexture2D* CETextureManager::LoadFromMemory(const uint8* pixels, uint32 width, uint32 height, uint32 createFlags,
                                              CEFormat format) {
	return nullptr;
}

CETextureCube* CETextureManager::CreateTextureCubeFromPanorama(CETexture2D* panoramaSource, uint32 cubeMapSize,
                                                               uint32 flags,
                                                               CEFormat format) {
	return nullptr;
}
