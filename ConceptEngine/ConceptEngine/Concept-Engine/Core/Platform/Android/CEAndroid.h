#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Android {
	class CEAndroid final : public Core::Generic::Platform::CEPlatform {
	public:
		CEAndroid();
		~CEAndroid() override;

		void Create() override;
		void CreateSystemWindow() override;
		void CreateSystemConsole() override;
		void CreateCursors() override;

		void Update() override;
	
	private:
		bool CreateInputManager() override;
	protected:
	private:
	};
}
