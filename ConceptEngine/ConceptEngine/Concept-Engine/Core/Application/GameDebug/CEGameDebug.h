#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Application::GameDebug {
	class CEGameDebug final : public CEGameCore {
	public:
		CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
		            Generic::Platform::Platform platform);
		CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		           Graphics::Main::Common::CEPlayground* playground);

		~CEGameDebug() override;

		bool Create() override;
		void Update(Time::CETimestamp DeltaTime) override;
	};
}
