#include "CEDxgiInfoManager.h"
#include "CEWindow.h"
#include "CEGraphics.h"
#include <dxgidebug.h>
#include <memory>

#include "CEConverters.h"

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if (FAILED(hresult = (hrcall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hresult)

CEDxgiInfoManager::CEDxgiInfoManager() {
	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	const auto hModDxgiDebug = LoadLibraryEx(CEConverters::ConvertCharArrayToLPCWSTR("dxgidebug.dll"), nullptr,
	                                         LOAD_LIBRARY_SEARCH_SYSTEM32);

	if (hModDxgiDebug == nullptr) {
		throw CEWIN_LAST_EXCEPTION();
	}
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(
		hModDxgiDebug, "DXGIGetDebugInterface")));
	if (DxgiGetDebugInterface == nullptr) {
		throw CEWIN_LAST_EXCEPTION();
	}

	HRESULT hresult;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&pDxgiInfoQueue)));
}

CEDxgiInfoManager::~CEDxgiInfoManager() {
	if (pDxgiInfoQueue != nullptr) {
		pDxgiInfoQueue->Release();
	}
}

void CEDxgiInfoManager::Set() noexcept {
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> CEDxgiInfoManager::GetMessages() const {
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++) {
		HRESULT hresult;
		SIZE_T messageLength;
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}
