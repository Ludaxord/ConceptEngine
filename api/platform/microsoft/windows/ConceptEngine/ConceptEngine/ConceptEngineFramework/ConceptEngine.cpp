#include "ConceptEngine.h"

using namespace Concept;

ConceptEngine::ConceptEngine(): apiType_(CEOSTools::CEGraphicsApiTypes::direct3d12) {
	Init();
	window_ = std::make_unique<CEWindow>(1920, 1080, "Concept Engine Editor", apiType_);
}

ConceptEngine::ConceptEngine(int windowWidth, int windowHeight, const char* windowName) {
	Init();
	window_ = std::make_unique<CEWindow>(windowWidth, windowHeight, windowName);
}

ConceptEngine::ConceptEngine(int width, int height, const char* name,
                             CEOSTools::CEGraphicsApiTypes graphicsApiType) : ConceptEngine(width, height, name) {
	apiType_ = graphicsApiType;
}

void ConceptEngine::Init() {
	InitSpdLog();
	static_logger_ = CreateLogger("ConceptEngine");
}

int ConceptEngine::Run() {
	return RunGraphics();
	return RunGameEngine();
}

int ConceptEngine::RunGameEngine() {
	return 0;
}

int ConceptEngine::RunGraphics() {
	window_->GetGraphics().LogSystemInfo();
	isAppRunning_ = true;
	while (true) {
		if (const auto ecode = CEWindow::ProcessMessages()) {
			return *ecode;
		}

		MakeFrame();
	}
}

void ConceptEngine::MakeFrame() {
	window_->GetGraphics().OnUpdate();
	window_->GetGraphics().OnRender();
}

Logger ConceptEngine::CreateLogger(const std::string& name) const {
	Logger logger = spdlog::get(name);
	if (!logger) {
		logger = logger_->clone(name);
		spdlog::register_logger(logger);
	}

	return logger;
}

Logger ConceptEngine::GetLogger() {
	return static_logger_;
}


void ConceptEngine::InitSpdLog() {

#if defined( _DEBUG )
	// Create a console window for std::cout
	CreateConsoleWindow();
#endif

	// Init spdlog.
	spdlog::init_thread_pool(8192, 1);
	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"logs/log.txt", 1024 * 1024 * 5, 3,
		true); // Max size: 5MB, Max files: 3, Rotate on open: true
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

	std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink, msvc_sink};
	logger_ = std::make_shared<spdlog::async_logger>("ConceptEngine", sinks.begin(), sinks.end(),
	                                                 spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	spdlog::register_logger(logger_);
	spdlog::set_default_logger(logger_);
	spdlog::flush_on(spdlog::level::info);
}

constexpr int MAX_CONSOLE_LINES = 500;

void ConceptEngine::CreateConsoleWindow() {
	// Allocate a console.
	if (AllocConsole()) {
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Increase screen buffer to allow more lines of text than the default.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
		consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
		SetConsoleCursorPosition(lStdHandle, {0, 0});

		// Redirect unbuffered STDOUT to the console.
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		FILE* fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		setvbuf(stdout, nullptr, _IONBF, 0);

		// Redirect unbuffered STDIN to the console.
		lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "r");
		freopen_s(&fp, "CONIN$", "r", stdin);
		setvbuf(stdin, nullptr, _IONBF, 0);

		// Redirect unbuffered STDERR to the console.
		lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
		setvbuf(stderr, nullptr, _IONBF, 0);

		// Clear the error state for each of the C++ standard stream objects. We
		// need to do this, as attempts to access the standard streams before
		// they refer to a valid target will cause the iostream objects to enter
		// an error state. In versions of Visual Studio after 2005, this seems
		// to always occur during startup regardless of whether anything has
		// been read from or written to the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}
}
