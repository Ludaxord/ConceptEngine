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
	// DXGetErrorDescription(hResult, CEConverters::ConvertCharArrayToLPCWSTR(buff), sizeof(buff));
	return buff;
}

const char* CEGraphics::DeviceRemovedException::GetType() const noexcept {
	return "Concept Engine Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

CEGraphics::CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType) : graphicsApiType(apiType) {
	ResolveSelectedGraphicsAPI();
}

void CEGraphics::EndFrame() {
}

void CEGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
                                   CEDefaultFigureTypes figureTypes) {
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
		graphics = new CEVulkanGraphics(hWnd);
		break;
	case CEOSTools::CEGraphicsApiTypes::opengl:
		graphics = new CEOpenGLGraphics(hWnd);
		break;
	case CEOSTools::CEGraphicsApiTypes::metal:
		graphics = new CEMetalGraphics(hWnd);
		break;
	default:
		graphics = new CEOpenGLGraphics(hWnd);
		break;
	}

	return graphics;
}

CEGraphicsManager CEGraphics::GetGraphicsManager() {
	return CEGraphicsManager();
}

void CEGraphics::PrintGraphicsVersion() {
	std::wstringstream wss;
	wss << "Graphics API" << std::endl;
	OutputDebugString(wss.str().c_str());
}

void CEGraphics::ResolveSelectedGraphicsAPI() {
	std::ostringstream oss;
	oss << "Graphics API type: ";
	oss << magic_enum::enum_name(graphicsApiType);
	oss << std::endl;
	OutputDebugString(CETools::ConvertCharArrayToLPCWSTR(oss.str().c_str()));
}
