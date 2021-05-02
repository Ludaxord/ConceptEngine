#pragma once

#include "wtypes.h"

static void GetScreenResolution(int& width, int& height) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	width = desktop.right;
	height = desktop.bottom;
}
