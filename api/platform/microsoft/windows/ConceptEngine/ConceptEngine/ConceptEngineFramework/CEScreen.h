#pragma once
#include "CEWindow.h"

enum class CEWindowState {
	Restored = 0,
	// The window has been resized.
	Minimized = 1,
	// The window has been minimized.
	Maximized = 2,
	// The window has been maximized.
};

enum class CEScreenTypes {
	Primary,
	AllCombined,
	WorkingArea,
	Specific
};


struct CEScreen {
	CEScreen(int vertical, int horizontal);

private:
	int horizontal = 0;
	int vertical = 0;
	double aspectRatio = 0;
	double refreshRate = 0;

public:
	double CalculateAspectRatio(int horizontal = 0, int vertical = 0);
	int GetRefreshRate();

	static CEScreen GetScreen(CEScreenTypes type = CEScreenTypes::Primary);
	static CEWindowState DecodeWindowState(WPARAM wParam);
};
