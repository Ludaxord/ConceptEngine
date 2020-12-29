#include "CEGraphics.h"

#include <sstream>
#include <d3dcompiler.h>
#include <iostream>
#include <magic_enum.hpp>

#include "CEDirect3DGraphics.h"
#include "CEMetalGraphics.h"
#include "CEOpenGLGraphics.h"
#include "CETools.h"
#include "CEVulkanGraphics.h"

CEGraphics::HResultException::HResultException(int line, const char* file,
                                               HRESULT hResult,
                                               std::vector<std::string> infoMsgs) noexcept : Exception(line, file),
	hResult(hResult) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* CEGraphics::HResultException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec <<
		" (" << (unsigned long)GetErrorCode() << ")" << std::endl << "[Error String] " << GetErrorMessage() << std::endl
		<< "[Error Description] " << GetErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info] \n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* CEGraphics::HResultException::GetType() const noexcept {
	return "Concept Engine Graphics Exception";
}

HRESULT CEGraphics::HResultException::GetErrorCode() const noexcept {
	return hResult;
}

std::string CEGraphics::HResultException::GetErrorMessage() const noexcept {
	// std::wstring ws(DXGetErrorString(hResult));
	// std::string errorMessage(ws.begin(), ws.end());
	// return errorMessage;
	return "";
}

std::string CEGraphics::HResultException::GetErrorInfo() const noexcept {
	return info;
}

CEGraphics::InfoException::InfoException(int line, const char* file,
                                         std::vector<std::string> infoMsgs) noexcept : Exception(line, file) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}

}

const char* CEGraphics::InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* CEGraphics::InfoException::GetType() const noexcept {
	return "Concept Engine Graphics Info Exception";
}

std::string CEGraphics::InfoException::GetErrorInfo() const noexcept {
	return info;
}


std::string CEGraphics::HResultException::GetErrorDescription() const noexcept {
	char buff[512];
	return buff;
}

const char* CEGraphics::DeviceRemovedException::GetType() const noexcept {
	return "Concept Engine Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

CEGraphics::CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height) : hWnd(hWnd),
	graphicsApiType(apiType), g_ClientWidth(width),
	g_ClientHeight(height) {
	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	m_assetsPath = assetsPath;
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	ResolveSelectedGraphicsAPI();
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

void CEGraphics::ChangeClearColor(float red, float green, float blue, float alpha) {
	clearColor[0] = red;
	clearColor[1] = green;
	clearColor[2] = blue;
	clearColor[3] = alpha;
}

void CEGraphics::SetFullscreen(bool fullscreen) {
}

CEGraphics* CEGraphics::GetGraphicsByApiType(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiTypes, int width, int height) {
	//TODO: Make CEGraphics Virtual and create implementation for every API -> Direct3D 11, Direct3D 12, Vulkan, OpenGL
	//TODO: Docs reference: https://en.cppreference.com/w/cpp/language/abstract_class, https://docs.microsoft.com/en-us/cpp/cpp/abstract-classes-cpp?view=msvc-160
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

void CEGraphics::PrintGraphicsVersion() {
	std::wstringstream wss;
	wss << "Graphics API" << std::endl;
	OutputDebugString(wss.str().c_str());
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


void CEGraphics::ResolveSelectedGraphicsAPI() {
	std::ostringstream oss;
	oss << "Graphics API type: ";
	oss << magic_enum::enum_name(graphicsApiType);
	oss << std::endl;
	OutputDebugString(CETools::ConvertCharArrayToLPCWSTR(oss.str().c_str()));
}
