#pragma once

#include "../../../Time/CETimestamp.h"


namespace ConceptEngine { namespace Render::Scene {
		class CEScene;
	}

	namespace Graphics::Main::Common {
		extern class CEPlayground* CreatePlayground();

		class CEPlayground {
		public:
			CEPlayground();
			virtual ~CEPlayground();

			virtual bool Create();
			virtual void Update(Time::CETimestamp DeltaTime);

			virtual bool Release();

			class Render::Scene::CEScene* Scene = nullptr;
		protected:
		private:
		};

		extern CEPlayground* GPlayground;
	}}
