#pragma once
#include <memory>

#include "Managers/CERendererManager.h"
#include "Managers/CETextureManager.h"
#include "Managers/CEMeshManager.h"
#include "RenderLayer/CEShaderCompiler.h"

#include "Rendering/CEDebugUI.h"
#include "../../Time/CETimestamp.h"

#include "Managers/CEGraphicsManager.h"


namespace ConceptEngine::Graphics::Main {
	class CEGraphics {
	public:
		CEGraphics();
		virtual ~CEGraphics() = default;
		virtual bool Create();
		virtual bool CreateManagers();
		virtual void Destroy();

		virtual void Update(Time::CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;

		static Rendering::CEDebugUI* GetDebugUI();

		Core::Common::CEManager* GetManager(Core::Common::CEManagerType type) const;

	protected:
		Managers::CEGraphicsManager* GraphicsManager;
		Managers::CETextureManager* TextureManager;
		Managers::CERendererManager* RendererManager;
		CEMeshManager* MeshManager;

		inline static std::unique_ptr<Rendering::CEDebugUI> DebugUI;	
	private:
		virtual bool CreateGraphicsManager() = 0;
		virtual bool CreateShaderCompiler() = 0;
		virtual bool CreateTextureManager() = 0;
		virtual bool CreateRendererManager() = 0;
		virtual bool CreateMeshManager() = 0;
		virtual bool CreateDebugUi() = 0;
	};
}
