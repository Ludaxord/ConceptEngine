#pragma once
#include "CEGraphics.h"

using namespace DirectX;
namespace wrl = Microsoft::WRL;

#define GFX_EXCEPT_NOINFO(hResult) CEGraphics::HResultException( __LINE__,__FILE__,(hResult) )
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hResult = (hrcall))) throw CEGraphics::HResultException(__LINE__, __FILE__, hResult)
#ifndef NDEBUG
#define GFX_EXCEPT(hResult) CEGraphics::HResultException( __LINE__, __FILE__, (hResult), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hResult = (hrcall))) throw GFX_EXCEPT(hResult)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, (hResult), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw CEGraphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hResult) CEGraphics::HResultException(__LINE__, __FILE__, (hResult))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hResult) CEGraphics::DeviceRemovedException(__LINE__, __FILE__, hResult)
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

class CEDirect3DGraphics : public CEGraphics {
public:
	CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType);
	~CEDirect3DGraphics() = default;
public:
	struct CED3DConstantBuffer {
		struct {
			dx::XMMATRIX transformation;
		};
	};

	CED3DConstantBuffer GetSampleConstantBuffer(float angle, float aspectRatioWidth, float aspectRatioHeight,
	                                            float x = 0.0f, float y = 0.0f, float z = 0.0f) {
		return CED3DConstantBuffer{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle) *
				// dx::XMMatrixScaling(aspectRatioHeight / aspectRatioWidth, 1.0f, 1.0f) *
				dx::XMMatrixTranslation(x, y, z + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f, aspectRatioHeight / aspectRatioWidth, 0.5f, 10.0f)
			)
		};
	};

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
};
