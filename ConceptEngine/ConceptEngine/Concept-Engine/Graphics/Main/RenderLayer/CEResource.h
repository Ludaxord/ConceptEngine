#pragma once

#include "../../../Core/Common/CERefCountedObject.h"

#include "../../../Core/Log/CELog.h"

#include <string>

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEResource : public CERefCountedObject {
	public:
		virtual ~CEResource() = default;

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

		virtual void DebugMessage() override {
			CE_LOG_VERBOSE("[CEResource]: Releasing Resource: " + GetName());
		}

	private:
		std::string Name;
	};
}
