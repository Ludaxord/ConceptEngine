#pragma once
#include "CEGraphics.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CEDirectXGraphics : public CEGraphics {

	public:
		CEDirectXGraphics(HWND hWnd, int width, int height);

		bool OnInit() override;
		void OnDestroy() override;
		void OnRender() override;
		void OnUpdate() override;
		void OnResize() override;
		void UpdatePerSecond(float second) override;
		bool LoadContent() override;
		void UnloadContent() override;
		void LogSystemInfo() override;
		void InitGui() override;
	protected:
		void RenderGui() override;
		void DestroyGui() override;
		void OnKeyPressed() override;
		void OnKeyReleased() override;
		void OnMouseMoved() override;
		void OnMouseButtonPressed() override;
		void OnMouseButtonReleased() override;
		void OnMouseWheel() override;
		void OnWindowDestroy() override;
	public:
		void SetFullscreen(bool fullscreen) override;

	};
}
