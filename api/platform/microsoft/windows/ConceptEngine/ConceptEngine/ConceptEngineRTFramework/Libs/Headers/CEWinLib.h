#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <comdef.h> // For _com_error class (used to decode HR result codes).

#include <fcntl.h> // For Translation mode constants _O_TEXT (https://docs.microsoft.com/en-us/cpp/c-runtime-library/translation-mode-constants)
#include <Shlwapi.h>  // For PathFindFileNameW (used in ReadDirectoryChanges.cpp)

#pragma comment( lib, "Shlwapi.lib" )

// STL
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>

namespace fs = std::filesystem;

// Common lock type
using scoped_lock = std::lock_guard<std::mutex>;

// gainput
#include <gainput/gainput.h>

// spdlog
#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
