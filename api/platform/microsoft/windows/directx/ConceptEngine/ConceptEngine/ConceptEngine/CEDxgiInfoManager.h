#pragma once
#include <string>

#include "CEWin.h"
#include <vector>

class CEDxgiInfoManager {
public:
	CEDxgiInfoManager();
	~CEDxgiInfoManager();
	CEDxgiInfoManager(const CEDxgiInfoManager&) = delete;
	CEDxgiInfoManager& operator=(const CEDxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};
