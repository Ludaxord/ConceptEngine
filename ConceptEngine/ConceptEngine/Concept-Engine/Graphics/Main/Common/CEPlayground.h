#pragma once

namespace ConceptEngine::Graphics::Main::Common {
	extern class CEPlayground* CreatePlayground();

	class CEPlayground {
	public:
		CEPlayground();
		virtual ~CEPlayground();

		virtual bool Create();
	protected:
	private:
	};

	extern CEPlayground* GPlayground;
}
