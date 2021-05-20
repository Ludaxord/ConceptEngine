#pragma once

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEScreenSpaceOcclusionRenderer {
	public: 
		bool Create(const CEFrameResources& resources);
		void Release();
		bool ResizeResources(CEFrameResources& resources);
	};
}
