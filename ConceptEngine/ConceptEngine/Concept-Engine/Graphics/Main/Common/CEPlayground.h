#pragma once

#include "../../../Time/CETimestamp.h"

namespace ConceptEngine::Graphics::Main::Common {
	extern class CEPlayground* CreatePlayground();

	class CEPlayground {
	public:
		CEPlayground();
		virtual ~CEPlayground();

		virtual bool Create();
		virtual void Update(Time::CETimestamp DeltaTime);
	protected:
	private:
	};

	extern CEPlayground* GPlayground;
}
