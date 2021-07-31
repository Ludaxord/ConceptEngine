#pragma once

#include "Core/Input/CEInputManager.h"

class CEWindowsInputManager final : public CEInputManager {
		public:
		CEWindowsInputManager();
		
		~CEWindowsInputManager() override;
		bool Create() override;
	protected:
		void InitKeyTable() override;

	private:
		void CreateNumScanCodeTableKeys();
		void CreateCharScanCodeTableKeys();
		void CreateFunctionScanCodeTableKeys();
		void CreateNumPadScanCodeTableKeys();
		void CreateActionScanCodeTableKeys();
};
