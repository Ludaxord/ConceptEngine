#pragma once
#include <imgui.h>
#include <memory>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXPipelineStateObject;
	class CEDirectXRootSignature;
	class CEDirectXShaderResourceView;
	class CEDirectXTexture;
	class CEDirectXDevice;
	class CEDirectXRenderTarget;
	class CEDirectXCommandList;
	namespace wrl = Microsoft::WRL;

	class CEDirectXGUI {

	public:
		/*
		 * Window message handler. Need to be called by application to allow ImGui to handle input messages.
		 */
		LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		/*
		 * Begin new ImGui frame. Do this before calling any ImGui functions that modifies ImGui's render context;
		 */
		void NewFrame();

		/*
		 * Render ImGui to given render target;
		 */
		void Render(const std::shared_ptr<CEDirectXCommandList>& commandList,
		            const CEDirectXRenderTarget& renderTarget);

		/*
		 * Destroy ImGui context.
		 */
		void Destroy();

		/*
		 * Set font scaling for ImGui (this should be called when window DPI scaling changes).
		 */
		void SetScaling(float scale);

	protected:
		CEDirectXGUI(CEDirectXDevice& device, HWND hWnd, const CEDirectXRenderTarget& renderTarget);
		virtual ~CEDirectXGUI();

	private:
		CEDirectXDevice& m_device;
		HWND m_hWnd;
		ImGuiContext* m_ImGuiContext;
		std::shared_ptr<CEDirectXTexture> m_fontTexture;
		std::shared_ptr<CEDirectXShaderResourceView> m_fontSRV;
		std::shared_ptr<CEDirectXRootSignature> m_rootSignature;
		std::shared_ptr<CEDirectXPipelineStateObject> m_pipelineState;
	};
}
