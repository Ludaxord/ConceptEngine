#pragma once
#include "RenderingCore.h"

#include "../Core/RefCountedObject.h"

#include "../Core/Application/Log.h"

class Resource : public RefCountedObject
{
public:
    virtual void* GetNativeResource() const { return nullptr; }

    virtual bool IsValid() const { return false; }

    virtual void SetName(const std::string& InName)
    {
        Name = InName;
    }

    const std::string& GetName() const { return Name; }

	virtual void DebugMessage() override {
		// LOG_WARNING("[Resource]: Releasing Resource: " + GetName());
	}

private:
    std::string Name;
};