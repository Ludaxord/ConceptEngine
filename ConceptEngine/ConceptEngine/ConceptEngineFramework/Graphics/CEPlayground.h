#pragma once
#include <string>

#include "../Game/CEKeyCodes.h"
#include "../Game/CETimer.h"

namespace ConceptEngineFramework::Graphics {
	struct CEGraphicsManager;

	class CEPlayground {
	public:
		virtual ~CEPlayground() = default;

		virtual void Init(CEGraphicsManager* manager) = 0;
		virtual void Create() = 0;
		virtual void Update(const CETimer& gt) = 0;
		virtual void Render(const CETimer& gt) = 0;
		virtual void Resize() = 0;
		virtual void OnMouseDown(Game::KeyCode key, int x, int y) = 0;
		virtual void OnMouseUp(Game::KeyCode key, int x, int y) = 0;
		virtual void OnMouseMove(Game::KeyCode key, int x, int y) = 0;
		virtual void OnMouseWheel(Game::KeyCode key, float wheelDelta, int x, int y) = 0;
		virtual void OnKeyUp(Game::KeyCode key, char keyChar) = 0;
		virtual void OnKeyDown(Game::KeyCode key, char keyChar) = 0;
	};
}
