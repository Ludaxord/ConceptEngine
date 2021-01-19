#pragma once
#include <imgui.h>
#include <memory>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEPipelineStateObject;
	class CERootSignature;
	class CEShaderResourceView;
	class CETexture;
	class CEDevice;
	class CERenderTarget;
	class CECommandList;
	namespace wrl = Microsoft::WRL;

	class CEGUI {

	public:
		/**
		 * Window message handler. Need to be called by application to allow ImGui to handle input messages.
		 */
		LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		/**
		 * Begin new ImGui frame. Do this before calling any ImGui functions that modifies ImGui's render context;
		 */
		void NewFrame();

		/**
		 * Render ImGui to given render target;
		 */
		void Render(const std::shared_ptr<CECommandList>& commandList,
		            const CERenderTarget& renderTarget);

		/**
		 * Destroy ImGui context.
		 */
		void Destroy();

		/**
		 * Set font scaling for ImGui (this should be called when window DPI scaling changes).
		 */
		void SetScaling(float scale);

	protected:
		CEGUI(CEDevice& device, HWND hWnd, const CERenderTarget& renderTarget);
		virtual ~CEGUI();

	private:
		CEDevice& m_device;
		HWND m_hWnd;
		ImGuiContext* m_ImGuiContext;
		std::shared_ptr<CETexture> m_fontTexture;
		std::shared_ptr<CEShaderResourceView> m_fontSRV;
		std::shared_ptr<CERootSignature> m_rootSignature;
		std::shared_ptr<CEPipelineStateObject> m_pipelineState;
	};
}
