#pragma once
#include <string>
#include <wrl.h>

#include "CEWin.h"
#include <vector>
#include <dxgidebug.h>


class CEDxgiInfoManager {
public:
	CEDxgiInfoManager();
	~CEDxgiInfoManager() = default;
	CEDxgiInfoManager(const CEDxgiInfoManager&) = delete;
	CEDxgiInfoManager& operator=(const CEDxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
