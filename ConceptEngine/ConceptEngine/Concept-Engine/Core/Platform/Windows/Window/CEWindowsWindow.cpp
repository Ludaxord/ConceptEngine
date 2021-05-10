#include "CEWindowsWindow.h"

#include "../../../../Memory/CEMemory.h"
#include "../CEWindows.h"
#include "../../../../ConceptEngine.h"

#include "../../../Log/CELog.h"

using namespace ConceptEngine::Core::Platform::Windows::Window;


CEWindowsWindow::CEWindowsWindow(): CEWindow() {
}

CEWindowsWindow::~CEWindowsWindow() {
}

bool CEWindowsWindow::Create() {
	if (!RegisterWindowClass()) {
		return false;
	}
	return true;
}

bool CEWindowsWindow::RegisterWindowClass() {
	WNDCLASS windowClass;
	Memory::CEMemory::Memzero(&windowClass);

	windowClass.hInstance = CEWindows::Instance;
	windowClass.lpszClassName = ConceptEngine::GetName().c_str();
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpfnWndProc = CEWindows::MessageProc;

	auto classAtm = RegisterClass(&windowClass);
	if (classAtm == 0) {
		CE_LOG_ERROR("[CEWindows]: Failed to register CEWindowsWindow Class\n");
		return false;
	}

	return true;
}

bool CEWindowsWindow::Create(const std::string& title, uint32 width, uint32 height,
                             Generic::Window::CEWindowStyle style) {
	if (!Create()) {
		return false;
	}

	return true;
}

void CEWindowsWindow::Show(bool maximized) {
}

void CEWindowsWindow::Minimize() {
}

void CEWindowsWindow::Maximize() {
}

void CEWindowsWindow::Close() {
}

void CEWindowsWindow::Restore() {
}

void CEWindowsWindow::ToggleFullscreen() {
}

bool CEWindowsWindow::IsValid() const {
	return true;
}

bool CEWindowsWindow::IsActiveWindow() const {
	return true;
}

void CEWindowsWindow::SetTitle(const std::string& title) {
}

void CEWindowsWindow::GetTitle(const std::string& outTitle) {
}

void CEWindowsWindow::SetWindowSize(const Generic::Window::CEWindowSize& shape, bool move) {
}

void CEWindowsWindow::GetWindowSize(Generic::Window::CEWindowSize& outShape) {
}

uint32 CEWindowsWindow::GetWidth() const {
	return 0;
}

uint32 CEWindowsWindow::GetHeight() const {
	return 0;
}
