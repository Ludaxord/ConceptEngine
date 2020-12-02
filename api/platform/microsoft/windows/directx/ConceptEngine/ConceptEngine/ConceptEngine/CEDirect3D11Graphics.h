#pragma once
#include "CEGraphics.h"

class CEDirect3D11Graphics : public CEGraphics {
public:
	CEDirect3D11Graphics(HWND hWnd);
	~CEDirect3D11Graphics() override = default;

public:
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;
private:
	static DXGI_SWAP_CHAIN_DESC GetSampleD311Descriptor(HWND hWnd) noexcept;

	CEFaceColorsConstantBuffer GetSampleFaceColorsConstantBuffer() {
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

	CEConstantBuffer GetSampleConstantBuffer(float angle, float aspectRatioWidth, float aspectRatioHeight,
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

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};
