#pragma once

#include "CEKey.h"
#include "../../../Common/CETypes.h"
#include "../../../Common/CEManager.h"
#include "../../../Common/CEEvents.h"
#include "../../../Containers/CEStaticArray.h"

namespace ConceptEngine::Core::Platform::Generic::Input {
	class CEInputManager : public Common::CEManager {
	public:
		CEInputManager();
		~CEInputManager() override;

		virtual bool Create() override;

		virtual bool IsKeyUp(CEKey keyCode);
		virtual bool IsKeyDown(CEKey keyCode);

		virtual CEKey ConvertFromScanCode(uint32 scanCode);
		virtual uint32 ConvertToScanCode(CEKey key);

	private:
		void InitKeyTable();

		void OnKeyPressed(const Common::CEKeyPressedEvent& Event);
		void OnKeyPressed(const Common::CEKeyReleasedEvent& Event);

		void OnWindowFocusChanged(const Common::CEWindowFocusChangedEvent& Event);

		Containers::CEStaticArray<bool, CEKey::Key_Count> KeyStates;
		Containers::CEStaticArray<CEKey, 512> ScanCodeTable;
		Containers::CEStaticArray<uint16, 512> KeyTable;
	};
}
