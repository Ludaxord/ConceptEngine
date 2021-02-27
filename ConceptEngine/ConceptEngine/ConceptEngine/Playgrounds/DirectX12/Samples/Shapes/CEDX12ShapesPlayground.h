#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12ShapesPlayground final : public CEDX12Playground {
	public:
		CEDX12ShapesPlayground();
		
		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;
		void UpdateCamera(const CETimer& gt) override;
		void OnMouseDown(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseUp(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseMove(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnKeyUp(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnKeyDown(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnMouseWheel(ConceptEngineFramework::Game::KeyCode key, float wheelDelta, int x, int y) override;
		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;
	};
}
