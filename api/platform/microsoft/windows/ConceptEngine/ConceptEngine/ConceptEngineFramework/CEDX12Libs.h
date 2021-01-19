#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>  // For CommandLineToArgvW

// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined( CreateWindow )
#undef CreateWindow
#endif

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl/client.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include "d3dx12.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>  // For ReportLiveObjects.

using namespace DirectX;

// STL Headers
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if defined( __cpp_lib_filesystem )
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

// Assimp header files.
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/anim.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// Helper functions
#include "CEHelper.h"