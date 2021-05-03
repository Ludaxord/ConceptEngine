#pragma once
namespace ConceptEngine::Core::Platform::Generic::Window {
	class CEScreen {
	public:
		friend class CEPlatform;
		CEScreen();
		virtual ~CEScreen();
	};
}
