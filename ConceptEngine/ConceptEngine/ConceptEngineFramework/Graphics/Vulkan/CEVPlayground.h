#pragma once
#include "../CEPlayground.h"

namespace ConceptEngineFramework::Graphics::Vulkan {
	class CEVPlayground : public CEPlayground {
	public:
		virtual void Init(CEGraphicsManager* manager) override;
		virtual void Create() override;
		virtual void Update(const CETimer& gt) override;
		virtual void Render(const CETimer& gt) override;
		virtual void Resize() override;
		virtual void OnMouseDown(Game::KeyCode key, int x, int y) override;
		virtual void OnMouseUp(Game::KeyCode key, int x, int y) override;
		virtual void OnMouseMove(Game::KeyCode key, int x, int y) override;
	};
}
