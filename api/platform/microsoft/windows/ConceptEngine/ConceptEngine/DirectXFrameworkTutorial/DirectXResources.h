#pragma once
#include <dxgi1_4.h>

namespace wrl = Microsoft::WRL;

class DirectXResources {
public:
	wrl::ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_swapChain.Get(); }
protected:
private:
	//SwapChain Objects
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
};
