#pragma once
namespace ConceptEngine::Core::Platform {
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
