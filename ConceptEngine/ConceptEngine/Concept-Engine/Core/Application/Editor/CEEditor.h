#pragma once

#include "../CECore.h"

#include "../../../Time/CETimer.h"

namespace ConceptEngine::Core::Application::Editor {
	class CEEditor final : public CECore {
	public:
		CEEditor(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEEditor(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		         Graphics::Main::Common::CEPlayground* playground);
		~CEEditor() override;

		bool Create() override;
		void Run() override;
		void Update(Time::CETimestamp DeltaTime) override;
		bool  Release() override;
		
		Time::CETimer Timer;
	};
}
