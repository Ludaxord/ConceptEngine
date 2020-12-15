#pragma once
#include "CEException.h"
#include <wrl.h>
#include <vector>
#include <complex>

#include "CEGraphicsManager.h"
#include "CEOSTools.h"

class CEGraphics {

public:
	enum class CEDefaultFigureTypes {
		triangle2d,
		cube3d
	};

public:
	struct CEVertex {
		struct {
			float x;
			float y;
			float z;
		} pos;
	};

	struct CEFaceColorsConstantBuffer {
		struct {
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

public:
	class Exception : public CEException {
		using CEException::CEException;
	};

	class HResultException : public Exception {
	public:
		HResultException(int line, const char* file, HRESULT hResult, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorMessage() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hResult;
		std::string info;
	};

	class InfoException : public Exception {
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	class DeviceRemovedException : HResultException {
		using HResultException::HResultException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

public:
	CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType);
	CEGraphics(const CEGraphics&) = delete;
	virtual CEGraphics& operator=(const CEGraphics&) = delete;
	virtual ~CEGraphics() = default;

public:
	virtual void EndFrame();
	virtual void ClearBuffer(float red, float green, float blue, float alpha = 1.0f) noexcept;
	virtual void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                               CEDefaultFigureTypes figureTypes = CEDefaultFigureTypes::triangle2d);

public:
	static CEGraphics* GetGraphicsByApiType(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiTypes, int width, int height);
	virtual CEGraphicsManager GetGraphicsManager();
	virtual void PrintGraphicsVersion();
protected:
	void ResolveSelectedGraphicsAPI();

protected:
	//TODO: after create Direct3D 11 port it to Direct3D 12 => Source: https://docs.microsoft.com/en-us/windows/win32/direct3d12/porting-from-direct3d-11-to-direct3d-12
	//TODO: add: Vulkan implementation => Source: https://www.khronos.org/registry/vulkan/specs/1.2/styleguide.html
	//TODO: add OpenGL implementation => Source: https://www.khronos.org/registry/OpenGL/index_gl.php

	CEOSTools::CEGraphicsApiTypes graphicsApiType;
};
