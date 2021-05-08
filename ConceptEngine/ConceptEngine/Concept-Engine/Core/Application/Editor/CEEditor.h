#pragma once

#include "../CECore.h"

#include "../../../Time/CETimer.h"

namespace ConceptEngine::Core::Application::Editor {
	class CEEditor final : public CECore {
	public:
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		         Graphics::Main::Common::CEPlayground* playground);
		~CEEditor() override;

		bool Create() override;
		int Run() override;
		void Update(Time::CETimestamp DeltaTime) override;
		bool  Release() override;
		
		Time::CETimer Timer;
	};
}
