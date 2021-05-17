#pragma once

#include "../../../Core/Common/CERefCountedObject.h"

#include <string>

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEResource : public Core::Common::CERefCountedObject {
	public:
		virtual void* GetNativeResource() const {
			return nullptr;
		}

		virtual bool IsValid() const {
			return false;
		}

		virtual void SetName(const std::string& name) {
			Name = name;
		}

		const std::string& GetName() const {
			return Name;
		}

	private:
		std::string Name;
	};
}
