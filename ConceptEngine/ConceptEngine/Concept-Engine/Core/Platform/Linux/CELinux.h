#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Linux {
	class CELinux final : public Core::Generic::Platform::CEPlatform {
	public:
		CELinux();
		~CELinux() override;

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
