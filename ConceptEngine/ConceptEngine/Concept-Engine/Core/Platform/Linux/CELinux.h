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
	protected:
	private:
	};
}
