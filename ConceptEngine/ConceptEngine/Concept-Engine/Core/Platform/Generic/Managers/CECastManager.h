#pragma once

#include "../../../../Graphics/Main/Managers/CEGraphicsManager.h"

#include "../../../Application/CECore.h"

#include "../Utilities/OSUtilities.h"

inline ConceptEngine::Graphics::Main::Managers::CEGraphicsManager* CastGraphicsManager() {
	return dynamic_cast<ConceptEngine::Graphics::Main::Managers::CEGraphicsManager*>(
		ConceptEngine::Core::Application::CECore::GetGraphics()->GetManager(
			ConceptEngine::Core::Common::CEManagerType::GraphicsManager));
}


#if defined(WINDOWS_PLATFORM)
#include "../../../../Graphics/DirectX12/Managers/CEDXManager.h"

inline ConceptEngine::Graphics::DirectX12::Managers::CEDXManager* CastDXManager() {
	return dynamic_cast<ConceptEngine::Graphics::DirectX12::Managers::CEDXManager*>(
		ConceptEngine::Core::Application::CECore::GetGraphics()->GetManager(
			ConceptEngine::Core::Common::CEManagerType::GraphicsManager));
}
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
