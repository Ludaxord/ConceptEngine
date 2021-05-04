#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Mac {
	class CEMac final : public Core::Generic::Platform::CEPlatform {
	public:
		CEMac();
		~CEMac() override;
		
		void Create() override;
		void CreateSystemWindow() override;
		void CreateSystemConsole() override;
		void CreateCursors() override;
	private:
		bool CreateInputManager() override;
	protected:
	private:
	};
}
