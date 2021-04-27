#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl/client.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>  // For ReportLiveObjects.
#include "../Graphics/DirectX12/Libraries/d3dx12.h"

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
// #include <assimp/anim.h>
// #include <assimp/Exporter.hpp>
// #include <assimp/Importer.hpp>
// #include <assimp/mesh.h>
// #include <assimp/postprocess.h>
// #include <assimp/ProgressHandler.hpp>
// #include <assimp/scene.h>

//tiny object loader
#include <tiny_obj_loader.h>

// Helper functions
#include <codecvt>
#include <comdef.h>
#include <exception>
#include <locale>
#include <wincodec.h>
#include <Windows.h> // For HRESULT

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl/client.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
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
// #include <assimp/anim.h>
// #include <assimp/Exporter.hpp>
// #include <assimp/Importer.hpp>
// #include <assimp/mesh.h>
// #include <assimp/postprocess.h>
// #include <assimp/ProgressHandler.hpp>
// #include <assimp/scene.h>

//tiny object loader
#include <tiny_obj_loader.h>
