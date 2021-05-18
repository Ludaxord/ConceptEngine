#pragma once

#include "../../../Core/Common/CEClassType.h"

#define CORE_OBJECT(TCEObject, TSuperClass) \
private: \
    typedef TCEObject This; \
    typedef TSuperClass Super; \
\
public: \
    static ConceptEngine::Core::Common::CEClassType* GetStaticClass() \
    { \
        static ConceptEngine::Core::Common::CEClassType ClassInfo(#TCEObject, Super::GetStaticClass(), sizeof(TCEObject)); \
        return &ClassInfo; \
    }

#define CORE_OBJECT_INIT() \
    this->SetClass(This::GetStaticClass())

namespace ConceptEngine::Graphics::Main::Objects {
	class CEObject {
	public:
		virtual ~CEObject() = default;

		const Core::Common::CEClassType* GetClass() const {
			return Class;
		}

		static const Core::Common::CEClassType* GetStaticClass() {
			static Core::Common::CEClassType classInfo("CEObject", nullptr, sizeof(CEObject));
			return &classInfo;
		}

	protected:
		void SetClass(const Core::Common::CEClassType* Class) {
			this->Class = Class;
		}

	private:
		const Core::Common::CEClassType* Class = nullptr;
	};
}

template <typename T>
bool IsSubClassOf(ConceptEngine::Graphics::Main::Objects::CEObject* object) {
	Assert(object != nullptr);
	Assert(object->GetClass() != nullptr);
	return object->GetClass()->IsSubClassOf<T>();
}

template <typename T>
T* Cast(ConceptEngine::Graphics::Main::Objects::CEObject* object) {
	return IsSubClassOf<T>(object) ? static_cast<T*>(object) : nullptr;
}
