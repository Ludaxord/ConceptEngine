#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <dxgidebug.h>

namespace wrl = Microsoft::WRL;

class CEDirect3DInfoManager {
public:
	CEDirect3DInfoManager();
	~CEDirect3DInfoManager() = default;
	CEDirect3DInfoManager(const CEDirect3DInfoManager&) = delete;
	CEDirect3DInfoManager& operator=(const CEDirect3DInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	wrl::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
