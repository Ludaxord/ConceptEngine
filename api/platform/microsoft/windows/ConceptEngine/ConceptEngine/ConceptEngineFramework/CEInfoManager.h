#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <dxgidebug.h>

namespace wrl = Microsoft::WRL;

class CEInfoManager {
public:
	CEInfoManager();
	~CEInfoManager() = default;
	CEInfoManager(const CEInfoManager&) = delete;
	CEInfoManager& operator=(const CEInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	wrl::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
