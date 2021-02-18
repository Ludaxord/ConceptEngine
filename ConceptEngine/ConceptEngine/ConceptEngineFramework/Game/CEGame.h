#pragma once
#include <activation.h>
#include <string>

namespace ConceptEngineFramework {
	class CEFramework;

	namespace Game {

		class CEGame {
		public:

			static CEGame& Get();
		protected:
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst);

		private:
			CEGame(std::wstring name, HINSTANCE hInst);

		};
	}
}
