#pragma once

#include "CETimestamp.h"
#include "../Core/Common/CETypes.h"

namespace ConceptEngine::Time {
	class CETimer {
	public:
		CETimer();
		
		void Update();

		void Reset() {
		}

		const CETimestamp& GetDeltaTime() const {
			return DeltaTime;
		}

		const CETimestamp& GetTotalTime() const {
			return TotalTime;
		}

	private:
		CETimestamp TotalTime = CETimestamp(0);
		CETimestamp DeltaTime = CETimestamp(0);
		uint64 LastTime = 0;
		uint64 Frequency = 0;
	};
}
