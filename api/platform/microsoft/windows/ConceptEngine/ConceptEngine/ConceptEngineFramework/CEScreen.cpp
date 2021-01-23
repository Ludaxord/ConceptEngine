#include "CEScreen.h"

using namespace Concept::GameEngine;

CEScreen::CEScreen(int vertical, int horizontal): vertical(vertical), horizontal(horizontal) {
	aspectRatio = CalculateAspectRatio(horizontal, vertical);
	refreshRate = GetRefreshRate();
}

double CEScreen::CalculateAspectRatio(int horizontal, int vertical) {
	return (double)(horizontal / vertical);
}

int CEScreen::GetRefreshRate() {
	DEVMODE mode;
	memset(&mode, 0, sizeof(mode));
	mode.dmSize = sizeof(mode);

	if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode))
		return 60;
	return mode.dmDisplayFrequency;
}

CEScreen CEScreen::GetScreen(CEScreenTypes type) {
	int horizontal = 0;
	int vertical = 0;
	switch (type) {
	case CEScreenTypes::Primary: {
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}
	break;
	case CEScreenTypes::AllCombined: {
	}
	break;
	case CEScreenTypes::Specific: {
	}
	break;
	case CEScreenTypes::WorkingArea: {
	}
	break;
	}
	auto screen = CEScreen(vertical, horizontal);
	return screen;
}

CEWindowState CEScreen::DecodeWindowState(WPARAM wParam) {
	CEWindowState windowState = CEWindowState::Restored;
	switch (wParam) {
	case SIZE_RESTORED:
		windowState = CEWindowState::Restored;
		break;
	case SIZE_MINIMIZED:
		windowState = CEWindowState::Minimized;
		break;
	case SIZE_MAXIMIZED:
		windowState = CEWindowState::Maximized;
		break;
	default:
		break;
	}
	return windowState;
}
