#include "CEGUI.h"

#include "ConceptEngine.h"


CEGUI::CEGUI(CEGUIType guiType, CEGraphicsApiType apiType): context(nullptr), guiType_(guiType), apiType_(apiType) {

}

CEGUI* CEGUI::Create(CEGUIType guiType, CEGraphicsApiType apiType) {
	return new CEGUI(guiType, apiType);
}

void CEGUI::InitDirect3D12ImGUI(HWND hWnd,
                                ID3D12Device* device,
                                ID3D12DescriptorHeap* heap,
                                D3D12_CPU_DESCRIPTOR_HANDLE imGuiCPUSrvHandle,
                                D3D12_GPU_DESCRIPTOR_HANDLE imGuiGPUSrvHandle,
                                int frameCount,
                                DXGI_FORMAT format) {
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGui::SetCurrentContext(context);
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(device, frameCount, format, heap, imGuiCPUSrvHandle, imGuiGPUSrvHandle);
}

void CEGUI::CreateSystemInfoWindow(CEOSTools::CESystemInfo systemInfo) {
	if (systemInfoWindow) {
		ImGui::Begin("Concept Engine Machine info", &systemInfoWindow);
		ImGui::Text("CPU: %s", systemInfo.CPUName.c_str());
		ImGui::Text("Threads: %f", systemInfo.CPUCores);
		ImGui::Text("GPU: %s", systemInfo.GPUName.c_str());
		ImGui::Text("Video Memory: %s MB", fmt::format("{:.4f}", systemInfo.VRamSize).c_str());
		ImGui::Text("RAM: %f MB", systemInfo.RamSize);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void CEGUI::CreateSceneEditorWindow() {
	if (sceneEditorWindow) {
		ImGui::Begin("Concept Engine Editor",&sceneEditorWindow);
		ImGui::ColorEdit4("Background Color", reinterpret_cast<float*>(&bgColor));
		ImGui::Checkbox("Toggle First Cube", &showCube1);
		ImGui::Checkbox("Toggle Second Cube", &showCube2);
		ImGui::Checkbox("Draw FPS Counter", &drawFPSCounter);
		ImGui::End();
	}
}

ImVec4 CEGUI::GetBgColor() const {
	return bgColor;
}

void CEGUI::CreateTopMenu() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {
			}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {
			}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("System Info")) {
				systemInfoWindow = !systemInfoWindow;
			}
			if (ImGui::MenuItem("Scene Editor")) {
				sceneEditorWindow = !sceneEditorWindow;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
