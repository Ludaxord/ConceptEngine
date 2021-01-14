#pragma once
#include <wrl.h>
#include <d3d12.h>

/*
 * ImGUI
 */
#include <memory>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "CEOSTools.h"

namespace wrl = Microsoft::WRL;
using CEGraphicsApiType = CEOSTools::CEGraphicsApiTypes;
using CEGUIType = CEOSTools::CEGUITypes;

class CEGUI {

public:
	CEGUI(CEGUIType guiType, CEGraphicsApiType apiType);
	CEGUI(const CEGUI&) = delete;
	virtual CEGUI& operator=(const CEGUI&) = delete;
	virtual ~CEGUI() = default;

public:
	static CEGUI* Create(CEGUIType guiType, CEGraphicsApiType apiType);

public:
	void InitDirect3D12ImGUI(HWND hWnd,
	                         ID3D12Device* device,
	                         ID3D12DescriptorHeap* heap,
	                         D3D12_CPU_DESCRIPTOR_HANDLE imGuiCPUSrvHandle,
	                         D3D12_GPU_DESCRIPTOR_HANDLE imGuiGPUSrvHandle,
	                         int frameCount,
	                         DXGI_FORMAT format);

	void CreateSystemInfoWindow(CEOSTools::CESystemInfo systemInfo);
	void CreateTopMenu();
	void CreateSceneEditorWindow();
	ImVec4 GetBgColor() const;
private:
	ImGuiContext* context;
	CEGUIType guiType_;
	CEGraphicsApiType apiType_;

	bool systemInfoWindow = true;
	bool sceneEditorWindow = true;
	ImVec4 bgColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

};
