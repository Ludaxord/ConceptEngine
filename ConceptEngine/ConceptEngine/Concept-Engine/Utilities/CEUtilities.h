#pragma once

namespace ConceptEngine {
	enum class EngineBoot {
		Game,
		Editor,
		GameDebug
	};

	static EngineBoot CurrentBoot;

	static EngineBoot GetEngineBoot() {
		return CurrentBoot;
	}

	static std::wstring InstanceName;

}
