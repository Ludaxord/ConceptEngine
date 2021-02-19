#pragma once
#include "CEPlayground.h"


namespace ConceptEngineFramework::Game::Playgrounds {

	class CEDXInitPlayground : public CEPlayground {
	public:
		CEDXInitPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);

		bool LoadContent() override;
		void UnloadContent() override;
	protected:
		void OnUpdate(UpdateEventArgs& e) override;
		void OnRender() override;
		void OnKeyPressed(KeyEventArgs& e) override;
		void OnKeyReleased(KeyEventArgs& e) override;
		void OnMouseMoved(MouseMotionEventArgs& e) override;
		void OnMouseWheel(MouseWheelEventArgs& e) override;
		void OnResize(ResizeEventArgs& e) override;
		void OnDPIScaleChanged(DPIScaleEventArgs& e) override;
	private:
	};
}
