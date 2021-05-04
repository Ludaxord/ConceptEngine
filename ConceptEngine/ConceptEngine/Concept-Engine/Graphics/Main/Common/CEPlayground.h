#pragma once

namespace ConceptEngine::Graphics::Main::Common {
	extern class CEPlayground* CreatePlayground();

	class CEPlayground {
	public:
		CEPlayground();
		virtual ~CEPlayground();

		virtual bool Create() = 0;
	protected:
	private:
	};

	extern CEPlayground* GPlayground;
}
