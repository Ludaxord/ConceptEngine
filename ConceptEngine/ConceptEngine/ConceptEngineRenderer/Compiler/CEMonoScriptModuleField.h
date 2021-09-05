#pragma once

extern "C" {
typedef struct _MonoClassField MonoClassField;
}

enum class CEMonoFieldType {
	None = 0,
	Float,
	Int,
	UnsignedInt,
	String,
	XMFLOAT2,
	XMFLOAT3,
	XMFLOAT4,
	ClassReference,
	Asset
};

inline const char* MonoFieldTypeToString(CEMonoFieldType Type) {
	switch (Type) {
	case CEMonoFieldType::Float: return "Float";
	case CEMonoFieldType::Int: return "Int";
	case CEMonoFieldType::UnsignedInt: return "UnsignedInt";
	case CEMonoFieldType::String: return "String";
	case CEMonoFieldType::XMFLOAT2: return "XMFLOAT2";
	case CEMonoFieldType::XMFLOAT3: return "XMFLOAT3";
	case CEMonoFieldType::XMFLOAT4: return "XMFLOAT4";
	}

	return "Unknown";
}

struct CEActorInstance;

struct CEMonoPublicField {
	std::string Name;
	std::string TypeName;
	CEMonoFieldType Type;

	CEMonoPublicField() = default;
	CEMonoPublicField(const std::string& InName, const std::string& InTypeName, CEMonoFieldType InFieldType);
	CEMonoPublicField(const CEMonoPublicField& PublicField);
	CEMonoPublicField(CEMonoPublicField&& PublicField);
	~CEMonoPublicField();

	CEMonoPublicField& operator=(const CEMonoPublicField& PublicField);

	void CopyStoredValueToRuntime(CEActorInstance& Instance);
	bool IsRuntimeAvailable() const;

	template <typename T>
	T GetStoredValue() const {
		T Value;
		GetStoredValueInternal(&Value);
		return Value;
	}

	template <typename T>
	T SetStoredValue(T Value) const {
		SetStoredValueInternal(&Value);
	}

	template <typename T>
	T GetRuntimeValue(CEActorInstance& Instance) const {
		T Value;
		GetRuntimeValueInternal(Instance, &Value);
		return Value;
	}

	template <typename T>
	T SetRuntimeValue(CEActorInstance& Instance, T Value) const {
		SetRuntimeValueInternal(Instance, &Value);
	}

	void SetStoredValueRaw(void* Src);

	void* GetStoredValueRaw() {
		return StoredValueBuffer;
	}

private:
	MonoClassField* ClassField;
	uint8* StoredValueBuffer = nullptr;

	uint8* AllocateBuffer(CEMonoFieldType Type);

	void SetStoredValueInternal(void* Value) const;
	void GetStoredValueInternal(void* OutValue) const;

	void SetRuntimeValueInternal(CEActorInstance& Instance, void* Value) const;
	void GetRuntimeValueInternal(CEActorInstance& Instance, void* OutValue) const;

	friend class CEMonoCompiler;
};


using CEMonoScriptModuleFieldMap = std::unordered_map<std::string, std::unordered_map<std::string, CEMonoPublicField>>;
