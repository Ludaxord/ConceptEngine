#pragma once
#include "../Memory/CEMemory.h"

namespace ConceptEngine::Math {
	struct CEColorF {

		CEColorF() : r(0), g(0), b(0), a(0) {

		}

		CEColorF(float R, float G, float B, float A)
			: r(R), g(G), b(B), a(A) {

		}

		CEColorF(const CEColorF& Other) {
			Memory::CEMemory::Memcpy(Elements, Other.Elements, sizeof(Elements));
		}

		void Set(float R, float G, float B, float A) {
			r = R;
			g = G;
			b = B;
			a = A;
		}

		CEColorF& operator=(const CEColorF& Other) {
			Memory::CEMemory::Memcpy(Elements, Other.Elements, sizeof(Elements));
			return *this;
		}

		union {
			float Elements[4];

			struct {
				float r;
				float g;
				float b;
				float a;
			};
		};
	};
}
