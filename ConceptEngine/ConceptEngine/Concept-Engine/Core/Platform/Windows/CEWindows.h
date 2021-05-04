#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Windows {
	class CEWindows final : public Core::Generic::Platform::CEPlatform {
	public:
		CEWindows();
		~CEWindows() override;
		
		void Create() override;
		void CreateSystemWindow() override;
		void CreateSystemConsole() override;
		void CreateCursors() override;
	protected:
	private:
	};
}
