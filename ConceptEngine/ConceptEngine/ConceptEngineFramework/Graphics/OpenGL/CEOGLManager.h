#pragma once
#include <memory>

#include "../CEGraphicsManager.h"

namespace ConceptEngineFramework::Graphics::OpenGL {
	class CEOGLManager : public CEGraphicsManager {

	public:
		void Create() override;
		void InitPlayground(CEPlayground* playground) override;
		void Destroy() override;
		void Resize() override;
		bool Initialized() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEOGLManager>;

		CEOGLManager();
		~CEOGLManager() = default;
	};
}
