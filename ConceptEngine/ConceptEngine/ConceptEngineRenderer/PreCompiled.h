#pragma once
#include <cassert>
#include <utility>
#include <algorithm>
#include <random>

// Libs
#ifdef PLATFORM_WINDOWS
    // Windows
    #define NOMINMAX
    #include "Windows/Windows.h"

    #include <dxgi1_6.h>
    #include <d3d12.h>

    #include <wrl/client.h>
#endif

#include "../Dependencies/imgui/imgui.h"

// Debug
#include "Debug/CEDebug.h"

// Core
#include "CEDefinitions.h"
#include "Core/Types.h"
#include "Core/CoreObject/ClassType.h"
#include "Core/CoreObject/CoreObject.h"
#include "Core/RefCountedObject.h"
#include "Core/CERef.h"

// Containers
#include "Core/Containers/Array.h"
#include "Core/Containers/ArrayView.h"
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/Function.h"
#include "Core/Containers/SharedPtr.h"
#include "Core/Containers/UniquePtr.h"
#include "Core/Containers/Utilities.h"

// Application
#include "Core/Application/Log.h"

// Utilities
#include "Utilities/HashUtilities.h"
#include "Utilities/StringUtilities.h"

// Math
#include "Math/Math.h"

// Memory
#include "Memory/CEMemory.h"
#include "Memory/New.h"

#include "yaml-cpp/yaml.h"
