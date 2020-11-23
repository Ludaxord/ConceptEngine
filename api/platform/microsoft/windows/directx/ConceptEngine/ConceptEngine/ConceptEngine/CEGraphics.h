#pragma once
#include "CEWin.h"
#include <d3d11.h>

#include "CEException.h"


class CEGraphics {
public:
	enum class CEGraphicsApiTypes {
		direct3d11,
		direct3d12,
		vulkan,
		opengl
	};

public:
	class Exception : CEException {
		using CEException::CEException;
	};

	class HResultException : CEException {
	public:
		HResultException(int line, const char* file, HRESULT hResult) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorMessage() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hResult;
	};

	class DeviceRemovedException : HResultException {
		using HResultException::HResultException;
	public:
		const char* GetType() const noexcept override;
	};

public:
	CEGraphics(HWND hWnd);
	CEGraphics(const CEGraphics&) = delete;
	CEGraphics& operator=(const CEGraphics&) = delete;
	~CEGraphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue, float alpha = 1.0f) noexcept;

public:
	static DXGI_SWAP_CHAIN_DESC GetDefaultD311Descriptor(HWND hWnd) noexcept;

private:
	//TODO: after create Direct3D 11 port it to Direct3D 12 => Source: https://docs.microsoft.com/en-us/windows/win32/direct3d12/porting-from-direct3d-11-to-direct3d-12
	//TODO: add: Vulkan implementation => Source: https://www.khronos.org/registry/vulkan/specs/1.2/styleguide.html
	//TODO: add OpenGL implementation => Source: https://www.khronos.org/registry/OpenGL/index_gl.php
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};
