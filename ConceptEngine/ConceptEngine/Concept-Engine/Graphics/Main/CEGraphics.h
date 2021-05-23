#pragma once
#include <memory>

#include "Managers/CEGraphicsManager.h"
#include "Managers/CERendererManager.h"
#include "Managers/CETextureManager.h"

#include "Rendering/CEDebugUI.h"
#include "../../Time/CETimestamp.h"


namespace ConceptEngine::Graphics::Main {


	class CEGraphics {
	public:
		friend class CECore;
		CEGraphics();
		virtual ~CEGraphics() = default;
		virtual bool Create();
		virtual bool CreateManagers();

		virtual void Update(Time::CETimestamp DeltaTime) = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
		virtual void Destroy() = 0;

		static Rendering::CEDebugUI* GetDebugUI();

		 Core::Common::CEManager* GetManager(Core::Common::CEManagerType type) const;
	
	protected:
		std::unique_ptr<Managers::CEGraphicsManager> GraphicsManager;
		std::unique_ptr<Managers::CETextureManager> TextureManager;
		std::unique_ptr<Managers::CERendererManager> RendererManager;
		RenderLayer::CEIShaderCompiler* Compiler = nullptr;

		static std::unique_ptr<Rendering::CEDebugUI> DebugUI;
	private:
		virtual bool CreateGraphicsManager() = 0;
		virtual bool CreateShaderCompiler() = 0;
		virtual bool CreateTextureManager() = 0;
		virtual bool CreateRendererManager() = 0;
		virtual bool CreateDebugUi() = 0;
	};
}
