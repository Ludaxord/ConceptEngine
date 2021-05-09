#pragma once
#include <sstream>
#include <string>

#include "CEConsoleObject.h"

#include "../../../Common/CETypes.h"

namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEConsoleVariable : public CEConsoleObject {
	public:
		virtual CEConsoleVariable* AsVariable() override {
			return this;
		}

		virtual void SetInt(int32 value) = 0;
		virtual void SetFloat(float value) = 0;
		virtual void SetBool(bool value) = 0;
		virtual void SetString(const std::string& value) = 0;

		virtual int32 GetInt() const = 0;
		virtual float GetFloat() const = 0;
		virtual bool GetBool() const = 0;
		virtual std::string GetString() const = 0;

		virtual bool IsInt() const = 0;
		virtual bool IsFloat() const = 0;
		virtual bool IsBool() const = 0;
		virtual bool IsString() const = 0;

		CEMulticastDelegate<CEConsoleVariable*> OnChangedDelegate;
	};

	template <typename T>
	class CEConsoleVariableEx : public CEConsoleVariable {
	public:
		CEConsoleVariableEx() : CEConsoleVariable(), Value() {

		}

		CEConsoleVariableEx(T initValue) : CEConsoleVariable(), Value(initValue) {

		}

		virtual void SetInt(int32 value) override {
			Value = (T)value;
			OnChanged();
		};

		virtual void SetFloat(float value) override {
			Value = (T)value;
			OnChanged();
		};

		virtual void SetBool(bool value) override {
			Value = (T)value;
			OnChanged();
		};

		virtual void SetString(const std::string& value) override;

		virtual int32 GetInt() const override {
			return (int32)Value;
		};

		virtual float GetFloat() const override {
			return (float)Value;
		};

		virtual bool GetBool() const override {
			return (bool)Value;
		};

		virtual std::string GetString() const override {
			return std::to_string(Value);
		};

		virtual bool IsInt() const override {
			return false;
		};

		virtual bool IsFloat() const override {
			return false;
		};

		virtual bool IsBool() const override {
			return false;
		};

		virtual bool IsString() const override {
			return false;
		};
	protected:
	private:
		void OnChanged() {
			if (OnChangedDelegate.IsBound()) {
				OnChangedDelegate.Broadcast(this);
			}
		}

		T Value;
	};

	template <>
	inline void CEConsoleVariableEx<int32>::SetString(const std::string& value) {
		Value = atoi(value.c_str());
		OnChanged();
	}

	template <>
	inline bool CEConsoleVariableEx<int32>::IsInt() const {
		return true;
	}

	template <>
	inline void CEConsoleVariableEx<float>::SetString(const std::string& value) {
		Value = (float)atof(value.c_str());
		OnChanged();
	}

	template <>
	inline bool CEConsoleVariableEx<float>::IsFloat() const {
		return true;
	}

	template <>
	void CEConsoleVariableEx<bool>::SetString(const std::string& value) {
		std::string lower = value;
		for (char& c : lower) {
			c = (char)tolower(c);
		}

		int32 num = 0;
		std::istringstream stream(lower);
		stream >> num;

		if (stream.fail()) {
			stream.clear();
			stream >> std::boolalpha >> Value;
		}
		else {
			Value = (bool)num;
		}

		if (!stream.fail()) {
			OnChanged();
		}
	}

	template <>
	inline std::string CEConsoleVariableEx<bool>::GetString() const {
		return Value ? "true" : "false";
	}

	template <>
	inline bool CEConsoleVariableEx<bool>::IsBool() const {
		return true;
	}

	template <>
	inline void CEConsoleVariableEx<std::string>::SetInt(int32 value) {

	}

	template <>
	inline void CEConsoleVariableEx<std::string>::SetFloat(float value) {

	}

	template <>
	inline void CEConsoleVariableEx<std::string>::SetBool(bool value) {

	}

	template <>
	inline void CEConsoleVariableEx<std::string>::SetString(const std::string& value) {

	}

	template <>
	inline bool CEConsoleVariableEx<std::string>::IsString() const {
		return true;
	}

	template <>
	inline int32 CEConsoleVariableEx<std::string>::GetInt() const {
		return 0;
	}

	template <>
	inline float CEConsoleVariableEx<std::string>::GetFloat() const {
		return 0.0f;
	}

	template <>
	inline bool CEConsoleVariableEx<std::string>::GetBool() const {
		return false;
	}

	template <>
	inline std::string CEConsoleVariableEx<std::string>::GetString() const {
		return Value;
	}
}
