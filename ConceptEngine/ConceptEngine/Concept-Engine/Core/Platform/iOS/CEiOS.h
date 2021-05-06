#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::iOS {
	class CEiOS final : public Core::Generic::Platform::CEPlatform {
	public:
		CEiOS();
		~CEiOS() override;

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
