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

		std::wstring GetName() override;
		void SetName(std::wstring name) override;

		Logger GetLogger() const;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEConsole>;

		CEConsole(std::wstring windowName, int maxFileSizeInMB = 5, int maxFiles = 3, int maxConsoleLines = 500);
		CEConsole(Logger logger);
		~CEConsole() = default;

		Logger CreateLogger(const std::string& name) const;
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
