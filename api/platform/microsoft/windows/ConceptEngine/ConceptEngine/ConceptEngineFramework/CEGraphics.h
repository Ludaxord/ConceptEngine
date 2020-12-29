#pragma once
#include "CEException.h"
#include <wrl.h>
#include <vector>
#include <complex>

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

	template <typename T>
	struct CEIndex {
		explicit CEIndex(T indx): i(indx) {

		}

		T i;
	};

	template <typename T>
	struct CEBuffer {
		std::vector<CEVertex> vertices;
		std::vector<CEIndex<T>> indices;
	};

	template <typename T>
	struct CEVertexBuffer : CEBuffer<T> {
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
	CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEGraphics(const CEGraphics&) = delete;
	virtual CEGraphics& operator=(const CEGraphics&) = delete;
	virtual ~CEGraphics() = default;

public:
	virtual bool OnInit() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnRender() = 0;
	virtual void OnUpdate() = 0;

public:
	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;
protected:
	virtual void OnKeyPressed() = 0;
	virtual void OnKeyReleased() = 0;
	virtual void OnMouseMoved() = 0;
	virtual void OnMouseButtonPressed() = 0;
	virtual void OnMouseButtonReleased() = 0;
	virtual void OnMouseWheel() = 0;
	virtual void OnResize() = 0;
	virtual void OnWindowDestroy() = 0;

public:
	void ChangeClearColor(float red, float green, float blue, float alpha = 1.0f);
	virtual void SetFullscreen(bool fullscreen);

public:
	static CEGraphics* GetGraphicsByApiType(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiTypes, int width, int height);
	virtual void PrintGraphicsVersion();

	bool GetVSyncState();
	void ChangeVSyncState();
	bool GetTearingSupportedState();
	bool GetFullScreenState();
	bool IsInitialized();
protected:
	void ResolveSelectedGraphicsAPI();
	std::wstring ExePath();
	std::wstring GetAssetFullPath(LPCWSTR assetName);

protected:
	//TODO: after create Direct3D 11 port it to Direct3D 12 => Source: https://docs.microsoft.com/en-us/windows/win32/direct3d12/porting-from-direct3d-11-to-direct3d-12
	//TODO: add: Vulkan implementation => Source: https://www.khronos.org/registry/vulkan/specs/1.2/styleguide.html
	//TODO: add OpenGL implementation => Source: https://www.khronos.org/registry/OpenGL/index_gl.php
	HWND hWnd;
	CEOSTools::CEGraphicsApiTypes graphicsApiType;

	uint32_t g_ClientWidth = 1280;
	uint32_t g_ClientHeight = 720;
	static const UINT FrameCount = 2;
	//Swap Chain Present Methods
	// By default, enable V-Sync.
	// Can be toggled with the V key.
	bool g_VSync = true;
	bool g_TearingSupported = false;
	// By default, use windowed mode.
	// Can be toggled with the Alt+Enter or F11
	bool g_Fullscreen = false;
	bool g_IsInitialized = false;
	FLOAT clearColor[4] = {0.4f, 0.6f, 0.9f, 1.0f};
	std::wstring m_assetsPath;
	float m_aspectRatio;

};
