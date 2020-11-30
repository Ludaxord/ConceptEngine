#pragma once
#include "CEWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "CEException.h"
#include <vector>
#include "CEDxgiInfoManager.h"
#include <complex>
#include <DirectXMath.h>

namespace dx = DirectX;


class CEGraphics {
public:
	//TODO: Finish Direct3D 11 basic implementation of shaders and add usage of direct3d 12 and Vulkan
	//TODO: Resources: https://github.com/NVIDIAGameWorks/DxrTutorials, https://github.com/microsoft/DirectX-Graphics-Samples, https://github.com/KhronosGroup/Vulkan-Samples
	enum class CEGraphicsApiTypes {
		direct3d11,
		direct3d12,
		vulkan,
		opengl
	};

	enum class CEDefaultFigureTypes {
		triangle2d,
		cube3d
	};

public:
	struct CEVertex {
		struct {
			float x;
			float y;
			float z;
		} pos;
	};

	struct CEConstantBuffer {
		struct {
			dx::XMMATRIX transformation;
		};
	};

	struct CEFaceColorsConstantBuffer {
		struct {
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	CEFaceColorsConstantBuffer GetDefaultFaceColorsConstantBuffer() {
		return CEFaceColorsConstantBuffer{
			{
				{1.0f, 0.0f, 1.0f},
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
				{1.0f, 1.0f, 0.0f},
				{0.0f, 1.0f, 1.0f},
			}
		};
	}

	CEConstantBuffer GetDefaultConstantBuffer(float angle, float aspectRatioWidth, float aspectRatioHeight,
	                                          float x = 0.0f, float y = 0.0f, float z = 0.0f) {
		return CEConstantBuffer{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle) *
				// dx::XMMatrixScaling(aspectRatioHeight / aspectRatioWidth, 1.0f, 1.0f) *
				dx::XMMatrixTranslation(x, y, z + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f, aspectRatioHeight / aspectRatioWidth, 0.5f, 10.0f)
			)
		};
	};

public:
	class Exception : public CEException {
		using CEException::CEException;
	};

	class HResultException : public Exception {
	public:
		HResultException(int line, const char* file, HRESULT hResult, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorMessage() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hResult;
		std::string info;
	};

	class InfoException : public Exception {
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	class DeviceRemovedException : HResultException {
		using HResultException::HResultException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

public:
	CEGraphics(HWND hWnd);
	CEGraphics(const CEGraphics&) = delete;
	CEGraphics& operator=(const CEGraphics&) = delete;
	~CEGraphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue, float alpha = 1.0f) noexcept;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes = CEDefaultFigureTypes::triangle2d);
private:
#ifndef NDEBUG
	CEDxgiInfoManager infoManager;
#endif
public:
	static DXGI_SWAP_CHAIN_DESC GetDefaultD311Descriptor(HWND hWnd) noexcept;

private:
	//TODO: after create Direct3D 11 port it to Direct3D 12 => Source: https://docs.microsoft.com/en-us/windows/win32/direct3d12/porting-from-direct3d-11-to-direct3d-12
	//TODO: add: Vulkan implementation => Source: https://www.khronos.org/registry/vulkan/specs/1.2/styleguide.html
	//TODO: add OpenGL implementation => Source: https://www.khronos.org/registry/OpenGL/index_gl.php
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};
