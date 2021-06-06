#pragma once

#include "../../../../Graphics/Main/Managers/CEGraphicsManager.h"
#include "../../../../Graphics/Main/Rendering/CEDebugUI.h"

#include "../../../Application/CECore.h"

#include "../Utilities/OSUtilities.h"

template <typename T>
inline T* CastManager(ConceptEngine::Core::Common::CEManagerType type) {
	return dynamic_cast<T*>(ConceptEngine::Core::Application::CECore::GetGraphics()->GetManager(type));
}

inline ConceptEngine::Graphics::Main::Managers::CEGraphicsManager* CastGraphicsManager() {
	return CastManager<ConceptEngine::Graphics::Main::Managers::CEGraphicsManager>(ConceptEngine::Core::Common::CEManagerType::GraphicsManager);
}

inline ConceptEngine::Graphics::Main::CEMeshManager* CastMeshManager() {
	return CastManager<ConceptEngine::Graphics::Main::CEMeshManager>(ConceptEngine::Core::Common::CEManagerType::MeshManager);
}

inline ConceptEngine::Graphics::Main::Rendering::CEDebugUI* GetDebugUI() {
	return ConceptEngine::Core::Application::CECore::GetGraphics()->GetDebugUI();
}

#if defined(WINDOWS_PLATFORM)
#include "../../../../Graphics/DirectX12/Managers/CEDXManager.h"
#include "../../../../Graphics/DirectX12/Managers/CEDXMeshManager.h"
#include "../../../../Graphics/DirectX12/Rendering/CEDXDebugUI.h"

inline ConceptEngine::Graphics::DirectX12::Managers::CEDXManager* CastDXManager() {
	return CastManager<ConceptEngine::Graphics::DirectX12::Managers::CEDXManager>(ConceptEngine::Core::Common::CEManagerType::GraphicsManager);
}

inline ConceptEngine::Graphics::DirectX12::Managers::CEDXMeshManager* CastDXMeshManager() {
	return CastManager<ConceptEngine::Graphics::DirectX12::Managers::CEDXMeshManager>(ConceptEngine::Core::Common::CEManagerType::MeshManager);
}

inline ConceptEngine::Graphics::DirectX12::Rendering::CEDXDebugUI* GetDXDebugUI() {
	return dynamic_cast<ConceptEngine::Graphics::DirectX12::Rendering::CEDXDebugUI*>(ConceptEngine::Core::Application::CECore::GetGraphics()->GetDebugUI());
}
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
