#pragma once

#include "../Main/CEGraphics.h"
#include "../../Time/CETimestamp.h"

namespace ConceptEngine::Graphics::OpenGL {
	class CEOpenGL final : public Main::CEGraphics {
	public:
		CEOpenGL();
		~CEOpenGL() override;
		
		bool Create() override;
		bool CreateManagers() override;
		
		void Update(Time::CETimestamp DeltaTime) override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	private:
		bool CreateRendererManager() override;
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		bool CreateDebugUi() override;
	};
}
