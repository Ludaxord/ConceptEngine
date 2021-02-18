#pragma once

#include <spdlog/logger.h>

#include "CEScreen.h"

#include "../CEHeaderLibs.h"

namespace ConceptEngineFramework::Game {
	class CEGame;

	class CEConsole : public CEScreen {

	public:
		using Logger = std::shared_ptr<spdlog::logger>;

		void Create() override;
		void Destroy() override;
		void Show() override;
		void Hide() override;

	protected:
		friend CEGame;
		CEConsole(std::wstring windowName, int maxFileSizeInMB = 5, int maxFiles = 3, int maxConsoleLines = 500);
		~CEConsole() = default;

		void RegisterLogger();
		void RegisterConsole();
	private:
		std::wstring m_windowName;
		int m_maxFileSizeInMB;
		int m_maxFiles;
		int m_maxConsoleLines;

		Logger m_logger;
	};
}
