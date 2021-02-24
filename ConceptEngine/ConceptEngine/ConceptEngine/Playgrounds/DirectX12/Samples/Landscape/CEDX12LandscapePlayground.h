#pragma once
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
using namespace ConceptEngineFramework::Graphics::DirectX12;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12LandscapePlayground final : public CEDX12Playground {
	public:
		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;
		void OnMouseDown(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseUp(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseMove(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnKeyUp(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnKeyDown(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnMouseWheel(ConceptEngineFramework::Game::KeyCode key, float wheelDelta, int x, int y) override;

	private:
		void BuildShadersAndInputLayout();

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shadersMap;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
	};
}
