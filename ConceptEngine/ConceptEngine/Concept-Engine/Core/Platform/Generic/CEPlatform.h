#pragma once
namespace ConceptEngine::Core::Generic::Platform {
	enum class Platform {
		Android,
		iOS,
		Linux,
		Mac,
		Windows,
		Unknown
	};
	
	class CEPlatform {
	public:
		friend class CECore;
		CEPlatform() = default;
	};
}
