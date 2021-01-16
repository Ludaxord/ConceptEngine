#include "CEGraphics.h"

#include <sstream>
#include <d3dcompiler.h>
#include <iostream>
#include <magic_enum.hpp>

#include "CEDirect3DGraphics.h"
#include "CEGUI.h"
#include "CEMetalGraphics.h"
#include "CEOpenGLGraphics.h"
#include "CETools.h"
#include "CEVulkanGraphics.h"


CEGraphics::CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height) :
	hWnd(hWnd),
	graphicsApiType(apiType),
	g_ClientWidth(width),
	g_ClientHeight(height) {
	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	m_assetsPath = assetsPath;
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	CEGUI::Create(CEOSTools::CEGUITypes::ImGUI, apiType);
}

std::wstring CEGraphics::ExePath() {
	TCHAR buffer[MAX_PATH] = {0};
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

// Helper function for resolving the full path of assets.
std::wstring CEGraphics::GetAssetFullPath(LPCWSTR assetName) {
	return m_assetsPath + assetName;
}

double CEGraphics::FPSFormula(uint64_t frameCounter, double elapsedSeconds) {
	return frameCounter / elapsedSeconds;
}

double CEGraphics::GetFPS() {
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	double fps = 0;
	if (elapsedSeconds > 1.0) {
		char buffer[500];
		fps = frameCounter / elapsedSeconds;

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
	return fps;
}

wchar_t* CEGraphics::CountFPS(bool displayLog) {
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	wchar_t* output = nullptr;
	if (elapsedSeconds > 1.0) {
		char buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		sprintf_s(buffer, 500, "FPS: %f\n", fps);
		output = CETools::ConvertCharArrayToLPCWSTR(buffer);
		if (displayLog) {
			OutputDebugString(output);
		}

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
	return output;
}

void CEGraphics::ChangeScreenSize(float width, float height) {
	g_ClientWidth = width;
	g_ClientHeight = height;
}

void CEGraphics::ChangeClearColor(float red, float green, float blue, float alpha) {
	clearColor[0] = red;
	clearColor[1] = green;
	clearColor[2] = blue;
	clearColor[3] = alpha;
}

void CEGraphics::SetFullscreen(bool fullscreen) {
}

CEGraphics* CEGraphics::GetGraphicsByApiType(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiTypes, int width, int height) {
	CEGraphics* graphics = nullptr;
	switch (apiTypes) {
	case CEOSTools::CEGraphicsApiTypes::direct3d11:
		graphics = new CEDirect3DGraphics(hWnd, apiTypes, width, height);
		break;
	case CEOSTools::CEGraphicsApiTypes::direct3d12:
		graphics = new CEDirect3DGraphics(hWnd, apiTypes, width, height);
		break;
	case CEOSTools::CEGraphicsApiTypes::vulkan:
		graphics = new CEVulkanGraphics(hWnd, width, height);
		break;
	case CEOSTools::CEGraphicsApiTypes::opengl:
		graphics = new CEOpenGLGraphics(hWnd, width, height);
		break;
	case CEOSTools::CEGraphicsApiTypes::metal:
		graphics = new CEMetalGraphics(hWnd, width, height);
		break;
	default:
		graphics = new CEOpenGLGraphics(hWnd, width, height);
		break;
	}

	return graphics;
}

bool CEGraphics::GetVSyncState() {
	return g_VSync;
}

void CEGraphics::ChangeVSyncState() {
	g_VSync = !g_VSync;
}

bool CEGraphics::GetTearingSupportedState() {
	return g_TearingSupported;
}

bool CEGraphics::GetFullScreenState() {
	return g_Fullscreen;
}

bool CEGraphics::IsInitialized() {
	return g_IsInitialized;
}
