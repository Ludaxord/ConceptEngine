#pragma once
#include <wrl.h>
#include <vector>
#include <complex>


#include "CEImGUI.h"
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
	CEGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEGraphics(const CEGraphics&) = delete;
	virtual CEGraphics& operator=(const CEGraphics&) = delete;
	virtual ~CEGraphics() = default;

public:
	virtual bool OnInit() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnRender() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnResize() = 0;

public:
	virtual void UpdatePerSecond(float second) = 0;
	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;
	virtual void LogSystemInfo() = 0;
	virtual void InitGui() = 0;

protected:
	virtual void RenderGui() = 0;
	virtual void DestroyGui() = 0;

	virtual void OnKeyPressed() = 0;
	virtual void OnKeyReleased() = 0;
	virtual void OnMouseMoved() = 0;
	virtual void OnMouseButtonPressed() = 0;
	virtual void OnMouseButtonReleased() = 0;
	virtual void OnMouseWheel() = 0;
	virtual void OnWindowDestroy() = 0;
public:
	void ChangeScreenSize(float width, float height);
	void ChangeClearColor(float red, float green, float blue, float alpha = 1.0f);
	virtual void SetFullscreen(bool fullscreen);

public:
	static CEGraphics* GetGraphicsByApiType(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiTypes, int width, int height);

	bool GetVSyncState();
	void ChangeVSyncState();
	bool GetTearingSupportedState();
	bool GetFullScreenState();
	bool IsInitialized();
protected:
	std::wstring ExePath();
	std::wstring GetAssetFullPath(LPCWSTR assetName);
	wchar_t* CountFPS(bool displayLog);
	double GetFPS();
	double FPSFormula(uint64_t frameCounter, double elapsedSeconds);

protected:
	HWND hWnd;
	CEOSTools::CEGraphicsApiTypes graphicsApiType;
	std::wstring m_assetsPath;

	FLOAT clearColor[4] = {0.4f, 0.6f, 0.9f, 1.0f};

	float m_aspectRatio;
	bool guiActive = false;
	bool mAppPaused = false; // is the application paused?
	bool mMinimized = false; // is the application minimized?
	bool mMaximized = false; // is the application maximized?
	bool mResizing = false; // are the resize bars being dragged?

	bool g_Fullscreen = false;
	bool g_IsInitialized = false;
	bool g_VSync = true;
	bool g_TearingSupported = false;

	static const UINT FrameCount = 3;
	uint32_t g_ClientWidth = 1920;
	uint32_t g_ClientHeight = 1080;


};
