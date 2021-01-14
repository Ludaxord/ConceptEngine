#include "CEDirect3DGraphics.h"


#include <fstream>
#include <magic_enum.hpp>


#include "CEDirect3DCube.h"
#include "CEGUI.h"
#include "CEHelper.h"
#include "CETools.h"
#include "CEWindow.h"
#include "ConceptEngine.h"
#include "d3dx12.h"


CEDirect3DGraphics::CEVertexPosColor triangleVertices[] =
{
	{{0.0f, 0.25f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.25f, -0.25f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

CEDirect3DGraphics::CEVertexPosColor quadVertices[] = {
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}}
};

CEDirect3DGraphics::CEVertexPosColor doubleQuadVertices[] = {
	//First quad
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	//Second quad
	{{-0.75f, 0.75f, 0.7f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{0.0f, 0.0f, 0.7f}, {1.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.75f, 0.0f, 0.7f}, {0.0f, 1.0f, 1.0f, 1.0f}},
	{{0.0f, 0.75f, 0.7f}, {1.0f, 1.0f, 0.0f, 1.0f}},
};

CEDirect3DGraphics::CEVertexPosTex CubesTexVertices[] = {
	// front face
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
	{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},

	// right side face
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},

	// left side face
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},

	// back face
	{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},

	// top face
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},

	// bottom face
	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
};

CEDirect3DGraphics::CEVertexPosColor cubesVertices[] = {
	// front face
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},

	// right side face
	{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},

	// left side face
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},

	// back face
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},

	// top face
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},

	// bottom face
	{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
};

//TODO: Test value fix CEDirect3DCube class to create vertices;
static const CEDirect3DGraphics::CEVertexPosColor g_Vertices[8] = {
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}, // 0
	{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 1
	{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}, // 2
	{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // 3
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // 4
	{{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}}, // 5
	{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // 6
	{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}} // 7
};

static const WORD g_Indicies[36] = {
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width,
                                       int height) : CEGraphics(hWnd, apiType, width, height),
                                                     m_rtvDescriptorSize(0),
                                                     m_Viewport(0.0f, 0.0f, static_cast<float>(width),
                                                                static_cast<float>(height)),
                                                     m_ScissorRect(0, 0, static_cast<LONG>(width),
                                                                   static_cast<LONG>(height)),
                                                     m_frameIndex(0),
                                                     m_FoV(45.0),
                                                     m_ContentLoaded(false) {

}

_Use_decl_annotations_

void CEDirect3DGraphics::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool requestHighPerformanceAdapter) const {
	*ppAdapter = nullptr;

	wrl::ComPtr<IDXGIAdapter1> adapter;

	wrl::ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
		for (
			UINT adapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true
					? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
					: DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter));
			++adapterIndex) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
		}
	}
	else {
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++
		     adapterIndex) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

bool CEDirect3DGraphics::CheckVSyncSupport() const {
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	wrl::ComPtr<IDXGIFactory4> factory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
		wrl::ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5))) {
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing)))) {
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}

// Load the rendering pipeline dependencies.
void CEDirect3DGraphics::LoadPipeline() {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	g_TearingSupported = CheckVSyncSupport();
	// Initialize the global window rect variable.
	::GetWindowRect(hWnd, &g_WindowRect);

}

// Load the sample assets.
void CEDirect3DGraphics::LoadAssets() {

}

void CEDirect3DGraphics::LoadBonus() {
	auto nDriverVersion = adapterDescription_.AdapterLuid;
	auto deviceId = adapterDescription_.DeviceId;
	auto revision = adapterDescription_.Revision;
	auto vendorId = adapterDescription_.VendorId;

	WORD nProduct = HIWORD(nDriverVersion.HighPart);
	WORD nVersion = LOWORD(nDriverVersion.HighPart);
	WORD nSubVersion = HIWORD(nDriverVersion.LowPart);
	WORD nBuild = LOWORD(nDriverVersion.LowPart);
	ConceptEngine::GetLogger()->info("Direct3D 12 Adapter Created");

	ConceptEngine::GetLogger()->info("GPU: {}, Video Memory: {:.4} MB", systemInfo_.GPUName,
	                                 fmt::format("{:.2f}", systemInfo_.VRamSize));
	ConceptEngine::GetLogger()->info("CPU: {}, Threads: {}", systemInfo_.CPUName, systemInfo_.CPUCores);
	ConceptEngine::GetLogger()->info("RAM: {}", fmt::format("{:.2f}", systemInfo_.RamSize));

	ConceptEngine::GetLogger()->info("Driver: {} Build: {}.{}.{}", nProduct, nBuild, nVersion, nSubVersion);
	ConceptEngine::GetLogger()->info("DeviceID: {}", deviceId);
	ConceptEngine::GetLogger()->info("Revision: {}", revision);
	ConceptEngine::GetLogger()->info("nVendorID: {}", vendorId);

	ConceptEngine::GetLogger()->info("GUI Created: {}", guiActive);

}


void CEDirect3DGraphics::OnUpdate() {
	UpdatePerSecond(1.0);
	// UpdateMultiplierColors();
	UpdateCubesRotation();
	auto cc = m_Gui->GetBgColor();
	ChangeClearColor(cc.x, cc.y, cc.z, cc.w);
}

void CEDirect3DGraphics::UpdateCubesRotation() {
	double delta = timer.GetFrameDelta();

	XMMATRIX rotXMat = XMMatrixRotationX(0.0001f * delta);
	XMMATRIX rotYMat = XMMatrixRotationY(0.0002f * delta);
	XMMATRIX rotZMat = XMMatrixRotationZ(0.0003f * delta);

	XMMATRIX rotMat = XMLoadFloat4x4(&cube1RotMat) * rotXMat * rotYMat * rotZMat;
	XMStoreFloat4x4(&cube1RotMat, rotMat);

	XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube1Position));
	XMMATRIX worldMat = rotMat * translationMat;

	XMStoreFloat4x4(&cube1WorldMat, worldMat);


	XMMATRIX viewMat = XMLoadFloat4x4(&cameraViewMat);
	XMMATRIX projMat = XMLoadFloat4x4(&cameraProjMat);
	XMMATRIX wvpMat = XMLoadFloat4x4(&cube1WorldMat) * viewMat * projMat;
	XMMATRIX transposed = XMMatrixTranspose(wvpMat);
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed);

	memcpy(cbvGPUAddress[m_frameIndex], &cbPerObject, sizeof(cbPerObject));

	rotXMat = XMMatrixRotationX(0.0003f * delta);
	rotYMat = XMMatrixRotationY(0.0002f * delta);
	rotZMat = XMMatrixRotationZ(0.0001f * delta);

	rotMat = rotZMat * (XMLoadFloat4x4(&cube2RotMat) * (rotXMat * rotYMat));
	XMStoreFloat4x4(&cube2RotMat, rotMat);

	XMMATRIX translationOffsetMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube2PositionOffset));

	XMMATRIX scaleMat = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	worldMat = scaleMat * translationOffsetMat * rotMat * translationMat;

	wvpMat = XMLoadFloat4x4(&cube2WorldMat) * viewMat * projMat;
	transposed = XMMatrixTranspose(wvpMat);
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed);

	memcpy(cbvGPUAddress[m_frameIndex] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));
	XMStoreFloat4x4(&cube2WorldMat, worldMat);
}

int CEDirect3DGraphics::LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription,
                                              LPCWSTR filename, int& bytesPerRow) {
	HRESULT hr;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory* wicFactory;

	// reset decoder, frame, and converter, since these will be different for each image we load
	IWICBitmapDecoder* wicDecoder = NULL;
	IWICBitmapFrameDecode* wicFrame = NULL;
	IWICFormatConverter* wicConverter = NULL;

	bool imageConverted = false;

	if (wicFactory == NULL) {
		// Initialize the COM library
		CoInitialize(NULL);

		// create the WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) return 0;

		hr = wicFactory->CreateFormatConverter(&wicConverter);
		if (FAILED(hr)) return 0;
	}

	// load a decoder for the image
	hr = wicFactory->CreateDecoderFromFilename(
		filename, // Image we want to load in
		NULL, // This is a vendor ID, we do not prefer a specific one so set to null
		GENERIC_READ, // We want to read from this file
		WICDecodeMetadataCacheOnLoad,
		// We will cache the metadata right away, rather than when needed, which might be unknown
		&wicDecoder // the wic decoder to be created
	);
	if (FAILED(hr)) return 0;

	// get image from decoder (this will decode the "frame")
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return 0;

	// get wic pixel format of image
	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	// get size of image
	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return 0;

	// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
	// out how to implement the support yourself

	// convert wic pixel format to dxgi pixel format
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	// if the format of the image is not a supported dxgi format, try to convert it
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN) {
		// get a dxgi compatible wic format from the current image format
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

		// return if no dxgi compatible format was found
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

		// set the dxgi format
		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

		// make sure we can convert to the dxgi compatible format
		BOOL canConvert = FALSE;
		hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert) return 0;

		// do the conversion (wicConverter will contain the converted image)
		hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0,
		                              WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return 0;

		// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
		imageConverted = true;
	}

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
	bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * textureHeight; // total image size in bytes

	// allocate enough memory for the raw image data, and set imageData to point to that memory
	*imageData = (BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (imageData)
	if (imageConverted) {
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}
	else {
		// no need to convert, just copy data from the wic frame
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}

	// now describe the texture with the information we have obtained from the image
	resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0;
	// may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	resourceDescription.Width = textureWidth; // width of the texture
	resourceDescription.Height = textureHeight; // height of the texture
	resourceDescription.DepthOrArraySize = 1;
	// if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	resourceDescription.MipLevels = 1;
	// Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	resourceDescription.SampleDesc.Quality = 0;
	// The quality level of the samples. Higher is better quality, but worse performance
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

	// return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
	return imageSize;
}

DXGI_FORMAT CEDirect3DGraphics::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID) {
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

WICPixelFormatGUID CEDirect3DGraphics::GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID) {
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

int CEDirect3DGraphics::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat) {
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
}

CEDirect3DGraphics::Font CEDirect3DGraphics::LoadFont(LPCWSTR filename, int windowWidth, int windowHeight) {
	std::wifstream fs;
	fs.open(filename);

	Font font;
	std::wstring tmp;
	int startpos;

	// extract font name
	fs >> tmp >> tmp; // info face="Arial"
	startpos = tmp.find(L"\"") + 1;
	font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

	// get font size
	fs >> tmp; // size=73
	startpos = tmp.find(L"=") + 1;
	font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;
	// bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

	// get padding
	fs >> tmp; // padding=5,5,5,5 
	startpos = tmp.find(L"=") + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

	// get up padding
	startpos = tmp.find(L",") + 1;
	font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	font.leftpadding = std::stoi(tmp) / (float)windowWidth;

	fs >> tmp; // spacing=0,0

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp >> tmp; // common lineHeight=95
	startpos = tmp.find(L"=") + 1;
	font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp; // base=68
	startpos = tmp.find(L"=") + 1;
	font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get texture width
	fs >> tmp; // scaleW=512
	startpos = tmp.find(L"=") + 1;
	font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get texture height
	fs >> tmp; // scaleH=512
	startpos = tmp.find(L"=") + 1;
	font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get pages, packed, page id
	fs >> tmp >> tmp; // pages=1 packed=0
	fs >> tmp >> tmp; // page id=0

	// get texture filename
	std::wstring wtmp;
	fs >> wtmp; // file="Arial.png"
	startpos = wtmp.find(L"\"") + 1;
	font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

	auto fontImagePath = GetAssetFullPath(font.fontImage.c_str());
	font.fontImage = fontImagePath;

	// get number of characters
	fs >> tmp >> tmp; // chars count=97
	startpos = tmp.find(L"=") + 1;
	font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the character list
	font.CharList = new FontChar[font.numCharacters];

	for (int c = 0; c < font.numCharacters; ++c) {
		// get unicode id
		fs >> tmp >> tmp; // char id=0
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get x
		fs >> tmp; // x=392
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

		// get y
		fs >> tmp; // y=340
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

		// get width
		fs >> tmp; // width=47
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
		font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

		// get height
		fs >> tmp; // height=57
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
		font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

		// get xoffset
		fs >> tmp; // xoffset=-6
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get yoffset
		fs >> tmp; // yoffset=16
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get xadvance
		fs >> tmp; // xadvance=65
		startpos = tmp.find(L"=") + 1;
		font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get page
		// get channel
		fs >> tmp >> tmp; // page=0    chnl=0
	}

	// get number of kernings
	fs >> tmp >> tmp; // kernings count=96
	startpos = tmp.find(L"=") + 1;
	font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the kernings list
	font.KerningsList = new FontKerning[font.numKernings];

	for (int k = 0; k < font.numKernings; ++k) {
		// get first character
		fs >> tmp >> tmp; // kerning first=87
		startpos = tmp.find(L"=") + 1;
		font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get second character
		fs >> tmp; // second=45
		startpos = tmp.find(L"=") + 1;
		font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get amount
		fs >> tmp; // amount=-1
		startpos = tmp.find(L"=") + 1;
		int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		font.KerningsList[k].amount = (float)t / (float)windowWidth;
	}

	return font;
}

void CEDirect3DGraphics::RenderText(Font font, std::wstring text, XMFLOAT2 pos, XMFLOAT2 scale, XMFLOAT2 padding,
                                    XMFLOAT4 color) {
	// clear the depth buffer so we can draw over everything
	m_commandList->ClearDepthStencilView(m_DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f,
	                                     0, 0, nullptr);

	// set the text pipeline state object
	m_commandList->SetPipelineState(textPSO);

	// this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// set the text vertex buffer
	m_commandList->IASetVertexBuffers(0, 1, &textVertexBufferView[m_frameIndex]);

	// bind the text srv. We will assume the correct descriptor heap and table are currently bound and set
	m_commandList->SetGraphicsRootDescriptorTable(1, font.srvHandle);

	int numCharacters = 0;

	float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (font.leftpadding + font.rightpadding) * padding.x;
	float verticalPadding = (font.toppadding + font.bottompadding) * padding.y;

	// cast the gpu virtual address to a textvertex, so we can directly store our vertices there
	CETextVertex* vert = (CETextVertex*)textVBGPUAddress[m_frameIndex];

	wchar_t lastChar = -1; // no last character to start with

	for (int i = 0; i < text.size(); ++i) {
		wchar_t c = text[i];

		FontChar* fc = font.GetChar(c);

		// character not in font char set
		if (fc == nullptr)
			continue;

		// end of string
		if (c == L'\0')
			break;

		// new line
		if (c == L'\n') {
			x = topLeftScreenX;
			y -= (font.lineHeight + verticalPadding) * scale.y;
			continue;
		}

		// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
		if (numCharacters >= maxNumTextCharacters)
			break;

		float kerning = 0.0f;
		if (i > 0)
			kerning = font.GetKerning(lastChar, c);

		vert[numCharacters] = CETextVertex(color.x,
		                                   color.y,
		                                   color.z,
		                                   color.w,
		                                   fc->u,
		                                   fc->v,
		                                   fc->twidth,
		                                   fc->theight,
		                                   x + ((fc->xoffset + kerning) * scale.x),
		                                   y - (fc->yoffset * scale.y),
		                                   fc->width * scale.x,
		                                   fc->height * scale.y);

		numCharacters++;

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * scale.x;

		lastChar = c;
	}

	// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
	m_commandList->DrawInstanced(4, numCharacters, 0, 0);

}

void CEDirect3DGraphics::InitImGui() {
	const auto gui = CEGUI::Create(CEOSTools::CEGUITypes::ImGUI, graphicsApiType);
	std::unique_ptr<CEGUI> guiInstance(gui);
	m_Gui = std::move(guiInstance);

	auto imGuiSrvHandleSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto imGuiGPUSrvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 2,
	                                                       imGuiSrvHandleSize);

	CD3DX12_CPU_DESCRIPTOR_HANDLE imGuiCPUSrvHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 2,
	                                                imGuiSrvHandleSize);
	m_Gui->InitDirect3D12ImGUI(hWnd,
	                           m_device.Get(),
	                           mainDescriptorHeap.Get(),
	                           imGuiCPUSrvHandle,
	                           imGuiGPUSrvHandle,
	                           FrameCount,
	                           DXGI_FORMAT_R8G8B8A8_UNORM);
}

void CEDirect3DGraphics::RenderImGui() const {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	m_Gui->CreateSystemInfoWindow(systemInfo_);
	m_Gui->CreateSceneEditorWindow();
	m_Gui->CreateTopMenu();
	ImGui::Render();
}

void CEDirect3DGraphics::DestroyImGui() const {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CEDirect3DGraphics::DestroyGui() {
	DestroyImGui();
}

void CEDirect3DGraphics::InitGui() {
	InitImGui();
	guiActive = true;
}

void CEDirect3DGraphics::RenderGui() {
	RenderImGui();
}

void CEDirect3DGraphics::UpdateMultiplierColors() {
	// update app logic, such as moving the camera or figuring out what objects are in view
	static float rIncrement = 0.00002f;
	static float gIncrement = 0.00006f;
	static float bIncrement = 0.00009f;

	cbColorMultiplierData.colorMultiplier.x += rIncrement;
	cbColorMultiplierData.colorMultiplier.y += gIncrement;
	cbColorMultiplierData.colorMultiplier.z += bIncrement;

	if (cbColorMultiplierData.colorMultiplier.x >= 1.0 || cbColorMultiplierData.colorMultiplier.x <= 0.0) {
		cbColorMultiplierData.colorMultiplier.x = cbColorMultiplierData.colorMultiplier.x >= 1.0 ? 1.0 : 0.0;
		rIncrement = -rIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.y >= 1.0 || cbColorMultiplierData.colorMultiplier.y <= 0.0) {
		cbColorMultiplierData.colorMultiplier.y = cbColorMultiplierData.colorMultiplier.y >= 1.0 ? 1.0 : 0.0;
		gIncrement = -gIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.z >= 1.0 || cbColorMultiplierData.colorMultiplier.z <= 0.0) {
		cbColorMultiplierData.colorMultiplier.z = cbColorMultiplierData.colorMultiplier.z >= 1.0 ? 1.0 : 0.0;
		bIncrement = -bIncrement;
	}

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbColorMultiplierGPUAddress[m_frameIndex], &cbColorMultiplierData, sizeof(cbColorMultiplierData));

}

void CEDirect3DGraphics::UpdatePerSecond(float second) {
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	wchar_t* output = nullptr;
	if (elapsedSeconds > second) {
		auto fps = FPSFormula(frameCounter, elapsedSeconds);
		ConceptEngine::GetLogger()->info("FPS: {:.7}", fps);

		std::stringstream oss;
		oss << "Screen Width: " << g_ClientWidth << " Screen Height: " << g_ClientHeight << std::endl;
		// ConceptEngine::GetLogger()->info(oss.str().c_str());

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
}

void CEDirect3DGraphics::UpdatePipeline() {
	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = m_commandAllocators[m_frameIndex]->Reset();
	if (FAILED(hr)) {
		Running = false;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get());
	if (FAILED(hr)) {
		Running = false;
	}

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		                               D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
	                                        m_rtvDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// clear the depth/stencil buffer
	m_commandList->ClearDepthStencilView(m_DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f,
	                                     0, 0, nullptr);

	// set root signature
	m_commandList->SetGraphicsRootSignature(m_RootSignature.Get()); // set the root signature

	// set the descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// set the descriptor table to the descriptor heap (parameter 1, as constant buffer root descriptor is parameter index 0)
	m_commandList->SetGraphicsRootDescriptorTable(1, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	m_commandList->RSSetViewports(1, &m_Viewport); // set the viewports
	m_commandList->RSSetScissorRects(1, &m_ScissorRect); // set the scissor rects
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	m_commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	// set the vertex buffer (using the vertex buffer view)
	m_commandList->IASetIndexBuffer(&m_IndexBufferView);

	// first cube

	// set cube1's constant buffer
	m_commandList->
		SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[m_frameIndex]->GetGPUVirtualAddress());

	// draw first cube
	m_commandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

	// second cube

	// set cube2's constant buffer. You can see we are adding the size of ConstantBufferPerObject to the constant buffer
	// resource heaps address. This is because cube1's constant buffer is stored at the beginning of the resource heap, while
	// cube2's constant buffer data is stored after (256 bits from the start of the heap).
	// m_commandList->SetGraphicsRootConstantBufferView(
	// 	0, constantBufferUploadHeaps[m_frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);
	m_commandList->SetGraphicsRootConstantBufferView(
		0, constantBufferUploadHeaps[m_frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);

	// draw second cube
	m_commandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

	// draw the text
	RenderText(mrRobotFont, std::wstring(L"FPS: ") + std::to_wstring(timer.fps), XMFLOAT2(0.02f, 0.01f),
	           XMFLOAT2(2.0f, 2.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	if (guiActive) {
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());
	}

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		                               D3D12_RESOURCE_STATE_PRESENT));

	hr = m_commandList->Close();
	if (FAILED(hr)) {
		Running = false;
	}
}

void CEDirect3DGraphics::OnRender() {
	if (guiActive) {
		RenderGui();
	}

	HRESULT hr;

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue

	// create an array of command lists (only one command list here)
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};

	// execute the array of command lists
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	hr = m_commandQueue->Signal(m_fences[m_frameIndex].Get(), m_fenceValues[m_frameIndex]);
	if (FAILED(hr)) {
		Running = false;
	}

	// present the current backbuffer
	hr = m_swapChain->Present(0, 0);
	if (FAILED(hr)) {
		Running = false;
	}
}

void CEDirect3DGraphics::Resize(uint32_t width, uint32_t height) {

}

void CEDirect3DGraphics::SetFullscreen(bool fullscreen) {

}

bool CEDirect3DGraphics::OnInit() {
	// Check for DirectX Math library support.
	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		CreateDirect3D11(g_ClientWidth, g_ClientHeight);
		return true;
	}
	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d12) {
		CreateDirect3D12(g_ClientWidth, g_ClientHeight);
		return true;
	}
	std::ostringstream oss;
	oss << "No API for enum: ";
	oss << magic_enum::enum_name(graphicsApiType);
	oss << std::endl;
	throw CEException(__LINE__, oss.str().c_str());
	return false;
}

void CEDirect3DGraphics::OnDestroy() {
	DestroyGui();
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
}

bool CEDirect3DGraphics::LoadContent() {
	return true;
}

void CEDirect3DGraphics::UnloadContent() {
}

void CEDirect3DGraphics::OnKeyPressed() {
}

void CEDirect3DGraphics::OnKeyReleased() {
}

void CEDirect3DGraphics::OnMouseMoved() {
}

void CEDirect3DGraphics::OnMouseButtonPressed() {
}

void CEDirect3DGraphics::OnMouseButtonReleased() {
}

void CEDirect3DGraphics::OnMouseWheel() {
}

void CEDirect3DGraphics::OnWindowDestroy() {
}

CEGraphics::IGPUInfo CEDirect3DGraphics::GetGPUInfo() {
	DXGI_QUERY_VIDEO_MEMORY_INFO dqvmi;
	wrl::ComPtr<IDXGIAdapter3> m_dxgiAdapter3;
	ThrowIfFailed(m_dxgiAdapter.As(&m_dxgiAdapter3));
	m_dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &dqvmi);
	ID3DGPUInfo d3dGpuInfo;
	d3dGpuInfo.dqvmi = dqvmi;
	return static_cast<IGPUInfo>(d3dGpuInfo);
}

DXGI_ADAPTER_DESC CEDirect3DGraphics::GetAdapterDescription(wrl::ComPtr<IDXGIAdapter> dxgiAdapter) const {
	DXGI_ADAPTER_DESC desc = {0};
	// wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;
	// ThrowIfFailed(m_dxgiAdapter.As(&dxgiAdapter4));
	ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
	// ThrowIfFailed(dxgiAdapter4->GetDesc3(&desc));
	return desc;
}

void CEDirect3DGraphics::CreateDirect3D12(int width, int height) {
	// LoadPipeline();
	// LoadAssets();
	bool initialized = InitD3D12();
	std::wstringstream wss;
	wss << "Direct3D 12 Initialized: " << initialized << std::endl;
	OutputDebugStringW(wss.str().c_str());
}

IDXGIFactory4* CEDirect3DGraphics::CreateFactory() const {
	IDXGIFactory4* dxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
	return dxgiFactory;
}


/*
 * For now CreateAdapter support only 1 adapter.
 * Improve to multiple adapters.
 */
IDXGIAdapter1* CEDirect3DGraphics::CreateAdapter(IDXGIFactory4* dxgiFactory) const {
	IDXGIAdapter1* adapter;
	int adapterIndex = 0;
	bool adapterFound = false;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound) {
		throw std::exception("GPU not support Direct3D 12");
	}
	return adapter;
}

ID3D12Device* CEDirect3DGraphics::CreateDevice(IDXGIAdapter1* adapter) {
	ID3D12Device* device;
	ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	return device;
}

ID3D12CommandQueue* CEDirect3DGraphics::CreateCommandQueue(ID3D12Device* device) {
	ID3D12CommandQueue* commandQueue;
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&commandQueue)));
	return commandQueue;
}

DXGI_SAMPLE_DESC CEDirect3DGraphics::CreateSampleDescriptor(int multiSampleCount) {
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = multiSampleCount;
	return sampleDesc;
}

IDXGISwapChain3* CEDirect3DGraphics::CreateSwapChain(IDXGIFactory4* dxgiFactory,
                                                     ID3D12CommandQueue* commandQueue,
                                                     uint32_t screenWidth,
                                                     uint32_t screenHeight,
                                                     int bufferCount,
                                                     HWND outputWindow,
                                                     bool windowed,
                                                     DXGI_SAMPLE_DESC sampleDesc,
                                                     DXGI_SWAP_EFFECT swapEffect) {
	DXGI_MODE_DESC backBufferDesc = {};
	backBufferDesc.Width = screenWidth;
	backBufferDesc.Height = screenHeight;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = bufferCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = swapEffect;
	swapChainDesc.OutputWindow = outputWindow;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.Windowed = windowed;
	IDXGISwapChain* tempSwapChain;
	dxgiFactory->CreateSwapChain(commandQueue, &swapChainDesc, &tempSwapChain);

	IDXGISwapChain3* swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);
	return swapChain;
}

UINT CEDirect3DGraphics::GetFrameIndex(IDXGISwapChain3* swapChain) {
	return swapChain->GetCurrentBackBufferIndex();
}

ID3D12DescriptorHeap* CEDirect3DGraphics::CreateDescriptorHeap(ID3D12Device* device,
                                                               int descriptorCount,
                                                               D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                                               D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags) const {
	ID3D12DescriptorHeap* heap;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = descriptorCount;
	rtvHeapDesc.Type = heapType;
	rtvHeapDesc.Flags = heapFlags;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&heap)));
	return heap;
}

UINT CEDirect3DGraphics::GetDescriptorSize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType) {
	return device->GetDescriptorHandleIncrementSize(heapType);
}

std::vector<ID3D12Resource*> CEDirect3DGraphics::CreateRenderTargetView(ID3D12DescriptorHeap* heap,
                                                                        int bufferCount,
                                                                        IDXGISwapChain3* swapChain,
                                                                        ID3D12Device* device,
                                                                        UINT descriptorSize,
                                                                        UINT offsetInDescriptor) const {
	std::vector<ID3D12Resource*> renderTargets;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < bufferCount; i++) {
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));

		device->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);
		rtvHandle.Offset(offsetInDescriptor, descriptorSize);
	}

	return renderTargets;
}

std::vector<ID3D12CommandAllocator*> CEDirect3DGraphics::CreateCommandAllocators(
	ID3D12Device* device, int bufferCount) const {
	std::vector<ID3D12CommandAllocator*> commandAllocators;
	for (int i = 0; i < bufferCount; i++) {
		ThrowIfFailed(
			device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
	}
	return commandAllocators;
}

ID3D12GraphicsCommandList* CEDirect3DGraphics::CreateCommandList(ID3D12Device* device,
                                                                 std::vector<ID3D12CommandAllocator*> commandAllocators,
                                                                 UINT frameIndex) const {
	ID3D12GraphicsCommandList* commandList;
	ThrowIfFailed(device->CreateCommandList(0,
	                                        D3D12_COMMAND_LIST_TYPE_DIRECT,
	                                        commandAllocators[frameIndex],
	                                        nullptr,
	                                        IID_PPV_ARGS(&commandList)));
	return commandList;
}

std::vector<CEDirect3DGraphics::CEFence> CEDirect3DGraphics::CreateFence(ID3D12Device* device, int bufferCount) const {
	std::vector<CEFence> fences;
	for (int i = 0; i < bufferCount; i++) {
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&(fences[i]).fence)));
		fences[i].fenceValue = 0;
	}
	return fences;
}

HANDLE CEDirect3DGraphics::CreateFenceEvent() const {
	const HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		throw std::exception("Error creating fence event");
	}
	return fenceEvent;
}

D3D12_ROOT_DESCRIPTOR CEDirect3DGraphics::CreateRootDescriptor() {
	D3D12_ROOT_DESCRIPTOR rootDescriptor;
	rootDescriptor.RegisterSpace = 0;
	rootDescriptor.ShaderRegister = 0;
	return rootDescriptor;
}

std::vector<D3D12_DESCRIPTOR_RANGE> CEDirect3DGraphics::CreateDescriptorRange(
	int range, int descriptorNumber, int shaderRegister) {
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableRanges; // only one range right now
	for (int i = 0; i < range; i++) {
		descriptorTableRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorTableRanges[i].NumDescriptors = descriptorNumber;
		descriptorTableRanges[i].BaseShaderRegister = shaderRegister;
		descriptorTableRanges[i].RegisterSpace = 0; // space 0. can usually be zero
		descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}
	return descriptorTableRanges;
}

D3D12_ROOT_DESCRIPTOR_TABLE CEDirect3DGraphics::CreateDescriptorTable(
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableRanges) {
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = descriptorTableRanges.size();
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];
	return descriptorTable;
}

std::vector<D3D12_ROOT_PARAMETER> CEDirect3DGraphics::CreateRootParameters(
	D3D12_ROOT_DESCRIPTOR rootDescriptor, D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable) {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	if (rootDescriptor.RegisterSpace != NULL && rootDescriptor.ShaderRegister != NULL) {
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].Descriptor = rootDescriptor;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	}

	if (descriptorTable.pDescriptorRanges != nullptr && descriptorTable.NumDescriptorRanges != NULL) {
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[1].DescriptorTable = descriptorTable;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	return rootParameters;
}

D3D12_STATIC_SAMPLER_DESC CEDirect3DGraphics::CreateStaticSampler(int shaderRegister, int registerSpace,
                                                                  float minLevelOfDetail, float maxLevelOfDetail) {
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = minLevelOfDetail;
	sampler.MaxLOD = maxLevelOfDetail;
	sampler.ShaderRegister = shaderRegister;
	sampler.RegisterSpace = registerSpace;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	return sampler;
}

ID3D12RootSignature* CEDirect3DGraphics::CreateRootSignature(ID3D12Device* device,
                                                             std::vector<D3D12_ROOT_PARAMETER> rootParameters,
                                                             int staticSampler,
                                                             D3D12_STATIC_SAMPLER_DESC sampler) const {
	ID3D12RootSignature* rootSignature;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(rootParameters.size(),
	                       &rootParameters[0],
	                       1,
	                       &sampler,
	                       D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* errorBuff; // a buffer holding the error data if any
	ID3DBlob* signature;
	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff))) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		throw std::exception("Error while serializing root signature");
	}

	ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
	                                          IID_PPV_ARGS(&rootSignature)));
	return rootSignature;
}

ID3DBlob* CEDirect3DGraphics::CompileShaderFromFile(std::wstring shaderFilePath, const char* entryPoint,
                                                    const char* shaderCompileTarget) {
	ID3DBlob* errorBuff;
	ID3DBlob* shader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	if (FAILED(D3DCompileFromFile(shaderFilePath.c_str(),
		nullptr,
		nullptr,
		entryPoint,
		shaderCompileTarget,
		compileFlags,
		0,
		&shader,
		&errorBuff))) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return errorBuff;
	}

	return shader;
}

D3D12_SHADER_BYTECODE CEDirect3DGraphics::CreateShaderByteCode(ID3DBlob* shader) {
	D3D12_SHADER_BYTECODE shaderBytecode = {};
	shaderBytecode.BytecodeLength = shader->GetBufferSize();
	shaderBytecode.pShaderBytecode = shader->GetBufferPointer();
	return shaderBytecode;
}

D3D12_BLEND_DESC CEDirect3DGraphics::CreateBlendDescriptor() {
	D3D12_BLEND_DESC blendStateDesc = {};
	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;

	blendStateDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blendStateDesc;
}

ID3D12PipelineState* CEDirect3DGraphics::CreatePipelineState(ID3D12Device* device,
                                                             ID3D12RootSignature* rootSignature,
                                                             std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
                                                             D3D12_SHADER_BYTECODE vertexShaderByteCode,
                                                             D3D12_SHADER_BYTECODE pixelShaderByteCode,
                                                             DXGI_SAMPLE_DESC sampleDesc,
                                                             D3D12_DEPTH_STENCIL_DESC depthStencilDesc,
                                                             D3D12_BLEND_DESC blendStateDesc,
                                                             D3D12_RASTERIZER_DESC rasterizerDesc) const {
	ID3D12PipelineState* pipelineState;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = inputLayout.size();
	inputLayoutDesc.pInputElementDescs = &inputLayout[0];

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS = vertexShaderByteCode;
	psoDesc.PS = pixelShaderByteCode;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;

	psoDesc.BlendState = blendStateDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = depthStencilDesc;

	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	return pipelineState;
}

ID3D12Resource* CEDirect3DGraphics::CreateHeap(ID3D12Device* device,
                                               D3D12_RESOURCE_DESC* resourceDesc,
                                               D3D12_HEAP_TYPE heapType,
                                               D3D12_RESOURCE_STATES resourceState,
                                               D3D12_CLEAR_VALUE* clearValue, std::wstring heapName) const {
	ID3D12Resource* heap;
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(heapType),
		D3D12_HEAP_FLAG_NONE,
		resourceDesc,
		resourceState,
		clearValue,
		IID_PPV_ARGS(&heap)));
	heap->SetName(heapName.c_str());
	return heap;
}

UINT64 CEDirect3DGraphics::GetUploadBufferSize(ID3D12Device* device, D3D12_RESOURCE_DESC desc) {
	UINT64 uploadBufferSize;
	device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
	return uploadBufferSize;
}

void CEDirect3DGraphics::CopyToDefaultHeap(ID3D12GraphicsCommandList* commandList,
                                           ID3D12Resource* destinationHeap,
                                           ID3D12Resource* intermediateHeap,
                                           const BYTE* dataArray,
                                           int bufferSize,
                                           bool useSlicePitch,
                                           int slicePitch) const {
	if (!useSlicePitch) {
		slicePitch = bufferSize;
	}
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = dataArray;
	subResourceData.RowPitch = bufferSize;
	subResourceData.SlicePitch = slicePitch;

	UpdateSubresources(commandList, destinationHeap, intermediateHeap, 0, 0, 1, &subResourceData);
}

void CEDirect3DGraphics::TransitionToBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
                                            D3D12_RESOURCE_STATES stateBefore,
                                            D3D12_RESOURCE_STATES stateAfter,
                                            int barrierNumber) {
	commandList->ResourceBarrier(barrierNumber,
	                             &CD3DX12_RESOURCE_BARRIER::Transition(
		                             resource,
		                             stateBefore,
		                             stateAfter
	                             )
	);
}

void CEDirect3DGraphics::CreateDepthStencilView(ID3D12Device* device, ID3D12DescriptorHeap* heap,
                                                ID3D12Resource* buffer,
                                                D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilDesc) {
	device->CreateDepthStencilView(buffer, depthStencilDesc,
	                               heap->GetCPUDescriptorHandleForHeapStart());
}

std::tuple<int, D3D12_RESOURCE_DESC, BYTE*> CEDirect3DGraphics::LoadTexture(std::wstring texturePath) {
	D3D12_RESOURCE_DESC textureDesc;
	BYTE* imageData;
	int imageBytesPerRow;
	auto imageSize = LoadImageDataFromFile(&imageData, textureDesc, texturePath.c_str(), imageBytesPerRow);
	if (imageSize <= 0) {
		std::stringstream oss;
		oss << "Texture not loaded from path: " << texturePath.c_str() << std::endl;
		auto message = oss.str();
		throw std::exception(message.c_str());
	}
	return std::make_tuple(imageBytesPerRow, textureDesc, imageData);
}

void CEDirect3DGraphics::CreateShaderResourceView(ID3D12Device* device,
                                                  ID3D12Resource* buffer,
                                                  ID3D12DescriptorHeap* heap,
                                                  D3D12_RESOURCE_DESC textureDesc,
                                                  D3D12_SRV_DIMENSION viewDimension,
                                                  int mipLevels,
                                                  D3D12_CPU_DESCRIPTOR_HANDLE handle) const {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = viewDimension;
	srvDesc.Texture2D.MipLevels = mipLevels;

	D3D12_CPU_DESCRIPTOR_HANDLE outputHandle;
	if (heap != nullptr) {
		outputHandle = heap->GetCPUDescriptorHandleForHeapStart();
	}
	else {
		outputHandle = handle;
	}

	device->CreateShaderResourceView(buffer,
	                                 &srvDesc,
	                                 outputHandle);
}

std::tuple<int, D3D12_RESOURCE_DESC, BYTE*>
CEDirect3DGraphics::LoadFonts(std::wstring fontPath, int width, int height) {
	auto font = LoadFont(fontPath.c_str(), width, height);

	D3D12_RESOURCE_DESC fontTextureDesc;
	int fontImageBytesPerRow;
	BYTE* fontImageData;
	int fontImageSize = LoadImageDataFromFile(&fontImageData, fontTextureDesc, font.fontImage.c_str(),
	                                          fontImageBytesPerRow);
	if (fontImageSize <= 0) {
		std::stringstream oss;
		oss << "Font not loaded from path: " << fontPath.c_str() << std::endl;
		auto message = oss.str();
		throw std::exception(message.c_str());
	}

	return std::make_tuple(fontImageBytesPerRow, fontTextureDesc, fontImageData);
}

void CEDirect3DGraphics::InitCommandList(ID3D12GraphicsCommandList* commandList, ID3D12CommandQueue* commandQueue) {
	commandList->Close();
	ID3D12CommandList* ppCommandLists[] = {commandList};
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void CEDirect3DGraphics::IncrementFenceValue(ID3D12CommandQueue* commandQueue, std::vector<CEFence> fences,
                                             UINT frameIndex) {
	fences[frameIndex].fenceValue++;
	ThrowIfFailed(commandQueue->Signal(fences[frameIndex].fence, fences[frameIndex].fenceValue));
}

D3D12_VERTEX_BUFFER_VIEW CEDirect3DGraphics::CreateVertexBufferView(ID3D12Resource* buffer, UINT stride, UINT size) {
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = stride;
	vertexBufferView.SizeInBytes = size;
	return vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CEDirect3DGraphics::CreateIndexBufferView(ID3D12Resource* buffer, DXGI_FORMAT indexBufferFormat,
                                                                  UINT size) {
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	indexBufferView.Format = indexBufferFormat;
	indexBufferView.SizeInBytes = size;
	return indexBufferView;
}

CD3DX12_VIEWPORT CEDirect3DGraphics::CreateViewPort(int topLeftX, int topLeftY, int width, int height, float minDepth,
                                                    float maxDepth) const {
	CD3DX12_VIEWPORT viewPort;
	viewPort.TopLeftX = topLeftX;
	viewPort.TopLeftY = topLeftY;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = minDepth;
	viewPort.MaxDepth = maxDepth;
	return viewPort;
}

CD3DX12_RECT CEDirect3DGraphics::CreateScissorRect(int left, int right, int top, int bottom) const {
	CD3DX12_RECT scissorRect;
	scissorRect.left = left;
	scissorRect.top = top;
	scissorRect.right = right;
	scissorRect.bottom = bottom;
	return scissorRect;
}

XMFLOAT4X4 CEDirect3DGraphics::BuildProjection(float angleY, int width, int height, float nearZ, float farZ) {
	XMFLOAT4X4 projectionMatrix;
	XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(angleY, (float)width / (float)height, nearZ, farZ);
	XMStoreFloat4x4(&projectionMatrix, tmpMat);
	return projectionMatrix;

}

XMFLOAT4X4 CEDirect3DGraphics::CreateViewMatrix(XMFLOAT4 position, XMFLOAT4 target, XMFLOAT4 up) {
	XMFLOAT4X4 viewMatrix;
	XMVECTOR cPos = XMLoadFloat4(&position);
	XMVECTOR cTarg = XMLoadFloat4(&target);
	XMVECTOR cUp = XMLoadFloat4(&up);
	XMMATRIX tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
	XMStoreFloat4x4(&viewMatrix, tmpMat);
	return viewMatrix;
}

XMFLOAT4X4 CEDirect3DGraphics::CreateTranslationMatrix(XMFLOAT4 position, XMFLOAT4 offset) {
	XMFLOAT4X4 translationMatrix;
	XMVECTOR posVec = XMLoadFloat4(&position) + XMLoadFloat4(&offset);
	XMMATRIX tmpMat = XMMatrixTranslationFromVector(posVec);
	XMStoreFloat4x4(&translationMatrix, tmpMat);
	return translationMatrix;
}

void CEDirect3DGraphics::ResetCommandAllocators(std::vector<ID3D12CommandAllocator*> commandAllocators,
                                                UINT frameIndex) {
	ThrowIfFailed(commandAllocators[frameIndex]->Reset());
}

void CEDirect3DGraphics::ResetCommandList(ID3D12GraphicsCommandList* commandList,
                                          std::vector<ID3D12CommandAllocator*> commandAllocators,
                                          UINT frameIndex,
                                          ID3D12PipelineState* pipelineState) {
	ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex], pipelineState));
}

void CEDirect3DGraphics::SetRenderTarget(ID3D12GraphicsCommandList* commandList,
                                         D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetViewHandle,
                                         CD3DX12_CPU_DESCRIPTOR_HANDLE* depthStencilViewHandle,
                                         int renderTargetDescriptorsCount) {
	commandList->OMSetRenderTargets(renderTargetDescriptorsCount, renderTargetViewHandle, FALSE,
	                                depthStencilViewHandle);
}

void CEDirect3DGraphics::ClearRenderTargetView(ID3D12GraphicsCommandList* commandList,
                                               D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle, float clearColor[4],
                                               int rectNumber) const {
	commandList->ClearRenderTargetView(renderTargetViewHandle, clearColor, rectNumber, nullptr);
}

void CEDirect3DGraphics::ClearDepthStencilView(ID3D12GraphicsCommandList* commandList,
                                               ID3D12DescriptorHeap* depthStencilViewHeap,
                                               float depth,
                                               float stencil,
                                               int rectNumber) const {
	commandList->ClearDepthStencilView(depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart(),
	                                   D3D12_CLEAR_FLAG_DEPTH, depth,
	                                   stencil, rectNumber, nullptr);
}

void CEDirect3DGraphics::SetGraphicsRootSignature(ID3D12GraphicsCommandList* commandList,
                                                  ID3D12RootSignature* rootSignature) const {
	commandList->SetGraphicsRootSignature(rootSignature);
}

void CEDirect3DGraphics::SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList,
                                            std::vector<ID3D12DescriptorHeap*> descriptorHeaps) {
	commandList->SetDescriptorHeaps(descriptorHeaps.size(), &descriptorHeaps[0]);
}

void CEDirect3DGraphics::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList,
                                                        ID3D12DescriptorHeap* descriptorHeap, UINT rootParameterIndex) {
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex,
	                                            descriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void CEDirect3DGraphics::SetViewports(ID3D12GraphicsCommandList* commandList, D3D12_VIEWPORT* viewPort,
                                      UINT viewPortsNumber) {
	commandList->RSSetViewports(viewPortsNumber, viewPort);
}

void CEDirect3DGraphics::SetScissorRects(ID3D12GraphicsCommandList* commandList, D3D12_RECT* scissorRect,
                                         UINT scissorRectsNumber) {
	commandList->RSSetScissorRects(scissorRectsNumber, scissorRect);
}

void CEDirect3DGraphics::SetBuffers(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW* vertexBufferView,
                                    D3D12_INDEX_BUFFER_VIEW* indexBufferView, UINT vertexBufferViewsNumber,
                                    UINT vertexBufferViewStart) {
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(vertexBufferViewStart, vertexBufferViewsNumber, vertexBufferView);
	commandList->IASetIndexBuffer(indexBufferView);
}

void CEDirect3DGraphics::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList,
                                                           ID3D12Resource* constantBufferUploadHeap,
                                                           UINT rootParameterIndex) {
	commandList->
		SetGraphicsRootConstantBufferView(rootParameterIndex, constantBufferUploadHeap->GetGPUVirtualAddress());
}

void CEDirect3DGraphics::DrawObject(ID3D12GraphicsCommandList* commandList,
                                    UINT cubeIndices,
                                    int instanceCount,
                                    int startIndexLocation,
                                    int baseVertexLocation,
                                    int startInstanceLocation) {
	commandList->DrawIndexedInstanced(cubeIndices, instanceCount, startIndexLocation, baseVertexLocation,
	                                  startInstanceLocation);
}

void CEDirect3DGraphics::CloseCommandList(ID3D12GraphicsCommandList* commandList) {
	ThrowIfFailed(commandList->Close());
}

void CEDirect3DGraphics::Init() {
	auto* dxgiFactory = CreateFactory();
	auto* adapter = CreateAdapter(dxgiFactory);
	auto* device = CreateDevice(adapter);
	auto adapterDescription = GetAdapterDescription(adapter);
	auto* commandQueue = CreateCommandQueue(device);

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	auto* swapChain = CreateSwapChain(dxgiFactory, commandQueue, g_ClientWidth, g_ClientHeight, FrameCount, hWnd,
	                                  g_Fullscreen, sampleDesc);
	auto frameIndex = GetFrameIndex(swapChain);

	auto descriptorType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	auto* renderTargetViewDescHeap = CreateDescriptorHeap(device, FrameCount, descriptorType,
	                                                      D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	const auto renderTargetViewDescHeapSize = GetDescriptorSize(device, descriptorType);
	auto renderTargets = CreateRenderTargetView(renderTargetViewDescHeap, FrameCount, swapChain, device,
	                                            renderTargetViewDescHeapSize);
	auto commandAllocator = CreateCommandAllocators(device, FrameCount);
	auto* commandList = CreateCommandList(device, commandAllocator, frameIndex);
	auto fences = CreateFence(device, FrameCount);
	auto fenceEvent = CreateFenceEvent();

	const auto rootCBVDescriptor = CreateRootDescriptor();
	auto descriptorTableRanges = CreateDescriptorRange(1, 1, 0);
	auto descriptorTable = CreateDescriptorTable(descriptorTableRanges);
	auto sampler = CreateStaticSampler();
	auto rootParams = CreateRootParameters(rootCBVDescriptor, descriptorTable);

	auto rootSignature = CreateRootSignature(device, rootParams, 1, sampler);

	auto vertexShaderPath = GetAssetFullPath(L"CEVertexShader.hlsl");
	auto vertexShader = CompileShaderFromFile(vertexShaderPath, "main", "vs_5_0");

	auto vertexShaderByteCode = CreateShaderByteCode(vertexShader);

	auto pixelShaderPath = GetAssetFullPath(L"CEPixelShader.hlsl");
	auto pixelShader = CompileShaderFromFile(pixelShaderPath, "main", "ps_5_0");

	auto pixelShaderByteCode = CreateShaderByteCode(pixelShader);

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	auto pipelineState = CreatePipelineState(device, rootSignature, inputLayout, vertexShaderByteCode,
	                                         pixelShaderByteCode, sampleDesc);

	auto textVertexShaderPath = GetAssetFullPath(L"CETextVertexShader.hlsl");
	auto textVertexShader = CompileShaderFromFile(textVertexShaderPath, "main", "vs_5_0");

	auto textVertexShaderByteCode = CreateShaderByteCode(textVertexShader);

	auto textPixelShaderPath = GetAssetFullPath(L"CETextPixelShader.hlsl");
	auto textPixelShader = CompileShaderFromFile(textPixelShaderPath, "main", "ps_5_0");

	auto textPixelShaderByteCode = CreateShaderByteCode(textPixelShader);

	std::vector<D3D12_INPUT_ELEMENT_DESC> textInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	auto blendDesc = CreateBlendDescriptor();

	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	textDepthStencilDesc.DepthEnable = false;

	auto textPipelineState = CreatePipelineState(device, rootSignature, textInputLayout, textVertexShaderByteCode,
	                                             textPixelShaderByteCode, sampleDesc, textDepthStencilDesc, blendDesc);

	int vBufferSize = sizeof(CubesTexVertices);
	auto* vertexBuffer = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), D3D12_HEAP_TYPE_DEFAULT,
	                                D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

	auto* vertexBufferUpload = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), D3D12_HEAP_TYPE_UPLOAD,
	                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	CopyToDefaultHeap(commandList, vertexBuffer, vertexBufferUpload, reinterpret_cast<BYTE*>(CubesTexVertices),
	                  vBufferSize);
	TransitionToBuffer(commandList, vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
	                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	DWORD iList[] = {
		// front face
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle

		// left face
		4, 5, 6, // first triangle
		4, 7, 5, // second triangle

		// right face
		8, 9, 10, // first triangle
		8, 11, 9, // second triangle

		// back face
		12, 13, 14, // first triangle
		12, 15, 13, // second triangle

		// top face
		16, 17, 18, // first triangle
		16, 19, 17, // second triangle

		// bottom face
		20, 21, 22, // first triangle
		20, 23, 21, // second triangle
	};

	int iBufferSize = sizeof(iList);

	auto cubeIndicesNumber = sizeof(iList) / sizeof(DWORD);
	auto indexBuffer = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), D3D12_HEAP_TYPE_DEFAULT,
	                              D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	auto* indexBufferUpload = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), D3D12_HEAP_TYPE_UPLOAD,
	                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	CopyToDefaultHeap(commandList, indexBuffer, indexBufferUpload, reinterpret_cast<BYTE*>(iList),
	                  iBufferSize);
	TransitionToBuffer(commandList, indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
	                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	auto* depthStencilViewHeap = CreateDescriptorHeap(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
	                                                  D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	auto depthStencilBuffer = CreateHeap(
		device,
		&CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			g_ClientWidth,
			g_ClientHeight,
			1,
			0,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue
	);


	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	CreateDepthStencilView(device, depthStencilViewHeap, depthStencilBuffer, &depthStencilDesc);

	std::vector<ID3D12Resource*> constantBufferUpload;
	UINT8* constantBufferGPUAddress[FrameCount];
	ConstantBufferPerObject constantBufferPerObject;
	int constantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;

	for (int i = 0; i < FrameCount; ++i) {
		constantBufferUpload[i] = CreateHeap(
			device, &CD3DX12_RESOURCE_DESC::Buffer(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

		ZeroMemory(&constantBufferPerObject, sizeof(constantBufferPerObject));

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(constantBufferUpload[i]->Map(0, &readRange,
		                                           reinterpret_cast<void**>(&constantBufferGPUAddress[i])));

		memcpy(constantBufferGPUAddress[i], &constantBufferPerObject, sizeof(constantBufferPerObject));
		memcpy(constantBufferGPUAddress[i] + constantBufferPerObjectAlignedSize, &constantBufferPerObject,
		       sizeof(constantBufferPerObject));
	}

	int textureSize;
	D3D12_RESOURCE_DESC textureDesc;
	BYTE* imageData;
	std::tie(textureSize, textureDesc, imageData) = LoadTexture(GetAssetFullPath(L"leather_red_03_coll1_1k.png"));
	auto* textureBuffer = CreateHeap(device, &textureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST,
	                                 nullptr);

	UINT64 textureUploadBufferSize = GetUploadBufferSize(device, textureDesc);

	auto* textureBufferUpload = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize),
	                                       D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &imageData[0]; // pointer to our image data
	textureData.RowPitch = textureSize; // size of all our triangle vertex data
	textureData.SlicePitch = textureSize * textureDesc.Height; // also the size of our triangle vertex data

	CopyToDefaultHeap(commandList, textureBuffer, textureBufferUpload, &imageData[0], textureSize, true,
	                  textureSize * textureDesc.Height);
	TransitionToBuffer(commandList, textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
	                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 2);


	auto* descriptorHeap = CreateDescriptorHeap(device, 2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	                                            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	CreateShaderResourceView(device, textureBuffer, descriptorHeap, textureDesc, D3D12_SRV_DIMENSION_TEXTURE2D, 1);
	int fontSize;
	D3D12_RESOURCE_DESC fontDesc;
	BYTE* fontData;
	std::tie(fontSize, fontDesc, fontData) =
		LoadFonts(GetAssetFullPath(L"mr-robot.fnt"), g_ClientWidth, g_ClientHeight);
	Font mainFont;
	mainFont.textureBuffer = CreateHeap(device, &fontDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST,
	                                    nullptr);


	auto fontTextureUploadBufferSize = GetUploadBufferSize(device, fontDesc);
	auto fontTextureBufferUpload = CreateHeap(device, &CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize),
	                                          D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	CopyToDefaultHeap(commandList, mainFont.textureBuffer.Get(), fontTextureBufferUpload, fontData, fontSize, true,
	                  fontSize * fontDesc.Height);
	TransitionToBuffer(commandList, mainFont.textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
	                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	auto handleSize = GetUploadBufferSize(device, fontDesc);

	mainFont.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1,
	                                                   handleSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, handleSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
	fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	fontsrvDesc.Format = fontDesc.Format;
	fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	fontsrvDesc.Texture2D.MipLevels = 1;
	CreateShaderResourceView(device, mainFont.textureBuffer.Get(), descriptorHeap, fontDesc,
	                         D3D12_SRV_DIMENSION_TEXTURE2D, 1, srvHandle);

	std::vector<ID3D12Resource*> textFontVertexBuffer;
	UINT8* textVertexBufferGPUAddress[FrameCount];

	for (int i = 0; i < FrameCount; ++i) {
		textFontVertexBuffer[i] = CreateHeap(
			device,
			&CD3DX12_RESOURCE_DESC::Buffer(maxNumTextCharacters * sizeof(CETextVertex)),
			D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr
		);

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(textFontVertexBuffer[i]->Map(0, &readRange,
		                                           reinterpret_cast<void**>(&textVertexBufferGPUAddress[i])));
	}
	InitCommandList(commandList, commandQueue);
	IncrementFenceValue(commandQueue, fences, frameIndex);
	auto vertexBufferView = CreateVertexBufferView(vertexBuffer, sizeof(CEVertexPosTex), vBufferSize);

	std::vector<D3D12_VERTEX_BUFFER_VIEW> textVertexBufferViews;
	for (int i = 0; i < FrameCount; ++i) {
		textVertexBufferViews[i] = CreateVertexBufferView(textFontVertexBuffer[i], sizeof(CEVertexPosTex),
		                                                  maxNumTextCharacters * sizeof(CETextVertex));
	}
	auto indexBufferView = CreateIndexBufferView(indexBuffer, DXGI_FORMAT_R32_FLOAT, iBufferSize);
	auto viewport = CreateViewPort(0, 0, g_ClientWidth, g_ClientHeight, 0.0f, 1.0f);
	auto scissorRect = CreateScissorRect(0, 0, g_ClientWidth, g_ClientHeight);
	auto cameraProjectionMatrix = BuildProjection(45.0f * (3.14f / 180.0f), g_ClientWidth, g_ClientHeight, 0.1f,
	                                              1000.0f);

	auto cameraViewMatrix = CreateViewMatrix(XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
	                                         XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
	auto cube1WorldMatrix = CreateTranslationMatrix(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&cube2RotMat, XMMatrixIdentity()); // initialize cube2's rotation matrix to identity matrix
	auto cube2WorldMatrix = CreateTranslationMatrix(XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f), cube1Position);

}


//TODO: Fix
void CEDirect3DGraphics::OnResize() {
	HRESULT hr;
	WaitForPreviousFrame();
	hr = m_commandAllocators[m_frameIndex]->Reset();
	if (FAILED(hr)) {
		Running = false;
	}

	hr = m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get());
	if (FAILED(hr)) {
		Running = false;
	}

	for (int i = 0; i < FrameCount; ++i) {
		m_renderTargets[i].Reset();
	}
	m_DepthBuffer.Reset();

	ThrowIfFailed(m_swapChain->ResizeBuffers(FrameCount, g_ClientWidth, g_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
	                                         DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	m_frameIndex = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < FrameCount; i++) {
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		if (FAILED(hr)) {
			Running = false;
		}

		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);

	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, g_ClientWidth, g_ClientHeight, 1, 0, 1, 0,
		                              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthBuffer)
	);
	if (FAILED(hr)) {
		Running = false;
	}

	m_device->CreateDepthStencilView(m_DepthBuffer.Get(), &depthStencilDesc,
	                                 m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthBuffer.Get(),
	                                                                        D3D12_RESOURCE_STATE_COMMON,
	                                                                        D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	WaitForPreviousFrame();

	// Update the viewport transform to cover the client area.
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = g_ClientWidth;
	m_Viewport.Height = g_ClientHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = g_ClientWidth;
	m_ScissorRect.bottom = g_ClientHeight;
}


bool CEDirect3DGraphics::InitD3D12() {
	HRESULT hr;

	// -- Create the Device -- //

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return false;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

	bool adapterFound = false; // set this to true when a good one was found
	// 
	// find first hardware gpu that supports d3d 12
	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// we dont want a software device
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr)) {
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound) {
		Running = false;
		return false;
	}

	// Create the device
	hr = D3D12CreateDevice(
		adapter,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_device)
	);
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	adapterDescription_ = GetAdapterDescription(adapter);
	auto systemInfo = CEOSTools::GetEngineSystemInfo();
	std::wstring wDesc = adapterDescription_.Description;
	auto videoMem = static_cast<double>(adapterDescription_.DedicatedVideoMemory);
	std::string sDesc(wDesc.begin(), wDesc.end());
	systemInfo.GPUName = sDesc;
	systemInfo.VRamSize = videoMem;
	systemInfo_ = systemInfo;

	// -- Create a direct command queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = m_device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_commandQueue)); // create the command queue
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// -- Create the Swap Chain (double/tripple buffering) -- //

	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = g_ClientWidth; // buffer width
	backBufferDesc.Height = g_ClientHeight; // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = hWnd; // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = !g_Fullscreen;
	// set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;

	dxgiFactory->CreateSwapChain(
		m_commandQueue.Get(), // the queue will be flushed once the swap chain is created
		&swapChainDesc, // give it the swap chain description we created above
		&tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
	);

	m_swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < FrameCount; i++) {
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		if (FAILED(hr)) {
			Running = false;
			return false;
		}

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	// D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	// desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// desc.NumDescriptors = 3;
	// desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// if (m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK) {
	// 	Running = false;
	// 	return false;
	// }

	// -- Create the Command Allocators -- //

	for (int i = 0; i < FrameCount; i++) {
		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
		if (FAILED(hr)) {
			Running = false;
			return false;
		}
	}

	// -- Create a Command List -- //

	// create the command list with the first allocator
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), NULL,
	                                 IID_PPV_ARGS(&m_commandList));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < FrameCount; i++) {
		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fences[i]));
		if (FAILED(hr)) {
			Running = false;
			return false;
		}
		m_fenceValues[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		Running = false;
		return false;
	}

	// create root signature

	// create a root descriptor, which explains where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// create a descriptor range (descriptor table) and fill it out
	// this is a range of descriptors inside a descriptor heap
	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1]; // only one range right now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// this is a range of shader resource views (descriptors)
	descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
	descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
	descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// this appends the range to the end of the root signature descriptor tables

	// create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

	// create a root parameter for the root descriptor and fill it out
	D3D12_ROOT_PARAMETER rootParameters[2]; // only one parameter right now
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// our pixel shader will be the only shader accessing this parameter for now

	// fill out the parameter for our descriptor table. Remember it's a good idea to sort parameters by frequency of change. Our constant
	// buffer will be changed multiple times per frame, while our descriptor table will not be changed at all (in this tutorial)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	rootParameters[1].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// our pixel shader will be the only shader accessing this parameter for now

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 2 root parameters
	                       rootParameters, // a pointer to the beginning of our root parameters array
	                       1, // we have one static sampler
	                       &sampler, // a pointer to our static sampler (array)
	                       D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	                       // we can deny shader stages here for better performance
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
	                       D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* errorBuff; // a buffer holding the error data if any
	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff);
	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
	                                   IID_PPV_ARGS(&m_RootSignature));
	if (FAILED(hr)) {
		return false;
	}

	wrl::ComPtr<ID3DBlob> vertexShader;
	wrl::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	//Add Command in Properties to copy shaders.hlsl to debug file and compile it at runtime
	auto shadersFile = GetAssetFullPath(L"shaders.hlsl");
	// ThrowIfFailed(D3DCompileFromFile(shadersFile.c_str(), nullptr, nullptr, "VSMain",
	//                                  "vs_5_0", compileFlags, 0, &vertexShader, &errorBuff));
	auto vertexShaderPath = GetAssetFullPath(L"CEVertexShader.hlsl");
	hr = D3DCompileFromFile(vertexShaderPath.c_str(),
	                        nullptr,
	                        nullptr,
	                        "main",
	                        "vs_5_0",
	                        compileFlags,
	                        0,
	                        &vertexShader,
	                        &errorBuff);

	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

	// ThrowIfFailed(D3DCompileFromFile(shadersFile.c_str(), nullptr, nullptr, "PSMain",
	//                                  "ps_5_0", compileFlags, 0, &pixelShader, &errorBuff));
	auto pixelShaderPath = GetAssetFullPath(L"CEPixelShader.hlsl");
	hr = D3DCompileFromFile(pixelShaderPath.c_str(),
	                        nullptr,
	                        nullptr,
	                        "main",
	                        "ps_5_0",
	                        compileFlags,
	                        0,
	                        &pixelShader,
	                        &errorBuff);

	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		Running = false;
		return false;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = m_RootSignature.Get(); // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode;
	// structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff;
	// sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	// create the pso
	hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	//Text PSO
	auto textVertexShaderPath = GetAssetFullPath(L"CETextVertexShader.hlsl");

	ID3DBlob* textVertexShader;
	hr = D3DCompileFromFile(textVertexShaderPath.c_str(), nullptr, nullptr, "main", "vs_5_0",
	                        compileFlags, 0, &textVertexShader, &errorBuff);
	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		Running = false;
		return false;
	}
	// Create vertex buffer

	D3D12_SHADER_BYTECODE textVertexShaderBytecode = {};
	textVertexShaderBytecode.BytecodeLength = textVertexShader->GetBufferSize();
	textVertexShaderBytecode.pShaderBytecode = textVertexShader->GetBufferPointer();

	ID3DBlob* textPixelShader;
	auto textPixelShaderPath = GetAssetFullPath(L"CETextPixelShader.hlsl");
	hr = D3DCompileFromFile(textPixelShaderPath.c_str(), nullptr, nullptr, "main", "ps_5_0",
	                        compileFlags, 0, &textPixelShader, &errorBuff);
	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		Running = false;
		return false;
	}

	D3D12_SHADER_BYTECODE textPixelShaderBytecode = {};
	textPixelShaderBytecode.BytecodeLength = textPixelShader->GetBufferSize();
	textPixelShaderBytecode.pShaderBytecode = textPixelShader->GetBufferPointer();

	D3D12_INPUT_ELEMENT_DESC textInputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	D3D12_INPUT_LAYOUT_DESC textInputLayoutDesc = {};
	textInputLayoutDesc.NumElements = sizeof(textInputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	textInputLayoutDesc.pInputElementDescs = textInputLayout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC textPsoDesc = {};
	textPsoDesc.InputLayout = textInputLayoutDesc;
	textPsoDesc.pRootSignature = m_RootSignature.Get();
	textPsoDesc.VS = textVertexShaderBytecode;
	textPsoDesc.PS = textPixelShaderBytecode;
	textPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	textPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	textPsoDesc.SampleDesc = sampleDesc;
	textPsoDesc.SampleMask = 0xffffffff;
	textPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	D3D12_BLEND_DESC textBlendStateDesc = {};
	textBlendStateDesc.AlphaToCoverageEnable = FALSE;
	textBlendStateDesc.IndependentBlendEnable = FALSE;
	textBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;

	textBlendStateDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	textPsoDesc.BlendState = textBlendStateDesc;
	textPsoDesc.NumRenderTargets = 1;
	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	textDepthStencilDesc.DepthEnable = false;
	textPsoDesc.DepthStencilState = textDepthStencilDesc;

	hr = m_device->CreateGraphicsPipelineState(&textPsoDesc, IID_PPV_ARGS(&textPSO));

	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	int vBufferSize = sizeof(CubesTexVertices);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
		// from the upload heap to this heap
		nullptr,
		// optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_VertexBuffer));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(CubesTexVertices); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(m_commandList.Get(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Create index buffer

	// a quad (2 triangles)
	DWORD iList[] = {
		// ffront face
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle

		// left face
		4, 5, 6, // first triangle
		4, 7, 5, // second triangle

		// right face
		8, 9, 10, // first triangle
		8, 11, 9, // second triangle

		// back face
		12, 13, 14, // first triangle
		12, 15, 13, // second triangle

		// top face
		16, 17, 18, // first triangle
		16, 19, 17, // second triangle

		// bottom face
		20, 21, 22, // first triangle
		20, 23, 21, // second triangle
	};

	int iBufferSize = sizeof(iList);

	numCubeIndices = sizeof(iList) / sizeof(DWORD);

	// create default heap to hold index buffer
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
		nullptr, // optimized clear value must be null for this type of resource
		IID_PPV_ARGS(&m_IndexBuffer));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Index Buffer Resource Heap");

	// create upload heap to upload index buffer
	ID3D12Resource* iBufferUploadHeap;
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&iBufferUploadHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	vBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(m_commandList.Get(), m_IndexBuffer.Get(), iBufferUploadHeap, 0, 0, 1, &indexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Create the depth/stencil buffer

	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, g_ClientWidth, g_ClientHeight, 1, 0, 1, 0,
		                              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthBuffer)
	);
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	m_DSVHeap->SetName(L"Depth/Stencil Resource Heap");

	m_device->CreateDepthStencilView(m_DepthBuffer.Get(), &depthStencilDesc,
	                                 m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	// create the constant buffer resource heap
	// We will update the constant buffer one or more times per frame, so we will use only an upload heap
	// unlike previously we used an upload heap to upload the vertex and index data, and then copied over
	// to a default heap. If you plan to use a resource for more than a couple frames, it is usually more
	// efficient to copy to a default heap where it stays on the gpu. In this case, our constant buffer
	// will be modified and uploaded at least once per frame, so we only use an upload heap

	// first we will create a resource heap (upload heap) for each frame for the cubes constant buffers
	// As you can see, we are allocating 64KB for each resource we create. Buffer resource heaps must be
	// an alignment of 64KB. We are creating 3 resources, one for each frame. Each constant buffer is 
	// only a 4x4 matrix of floats in this tutorial. So with a float being 4 bytes, we have 
	// 16 floats in one constant buffer, and we will store 2 constant buffers in each
	// heap, one for each cube, thats only 64x2 bits, or 128 bits we are using for each
	// resource, and each resource must be at least 64KB (65536 bits)
	for (int i = 0; i < FrameCount; ++i) {
		// create resource for cube 1
		hr = m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			// this heap will be used to upload the constant buffer data
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),
			// size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
			D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
			nullptr, // we do not have use an optimized clear value for constant buffers
			IID_PPV_ARGS(&constantBufferUploadHeaps[i]));
		if (FAILED(hr)) {
			Running = false;
			return false;
		}
		constantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

		ZeroMemory(&cbPerObject, sizeof(cbPerObject));

		CD3DX12_RANGE readRange(0, 0);
		// We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

		// map the resource heap to get a gpu virtual address to the beginning of the heap
		hr = constantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbvGPUAddress[i]));

		// Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned. Our buffers are smaller than 256 bits,
		// so we need to add spacing between the two buffers, so that the second buffer starts at 256 bits from the beginning of the resource heap.
		memcpy(cbvGPUAddress[i], &cbPerObject, sizeof(cbPerObject)); // cube1's constant buffer data
		memcpy(cbvGPUAddress[i] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));
		// cube2's constant buffer data
	}

	// load the image, create a texture resource and descriptor heap

	// Load the image from file
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	auto imagePath = GetAssetFullPath(L"leather_red_03_coll1_1k.png");
	int imageSize = LoadImageDataFromFile(&imageData, textureDesc, imagePath.c_str(), imageBytesPerRow);

	// make sure we have data
	if (imageSize <= 0) {
		Running = false;
		return false;
	}

	debugImagePath = imagePath;

	// create a default heap where the upload heap will copy its contents into (contents being the texture)
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&textureDesc, // the description of our texture
		D3D12_RESOURCE_STATE_COPY_DEST,
		// We will copy the texture from the upload heap to here, so we start it out in a copy dest state
		nullptr, // used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&textureBuffer));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	textureBuffer->SetName(L"Texture Buffer Resource Heap");

	UINT64 textureUploadBufferSize;
	// this function gets the size an upload buffer needs to be to upload a texture to the gpu.
	// each row must be 256 byte aligned except for the last row, which can just be the size in bytes of the row
	// eg. textureUploadBufferSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
	//textureUploadBufferSize = (((imageBytesPerRow + 255) & ~255) * (textureDesc.Height - 1)) + imageBytesPerRow;
	m_device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

	// now we create an upload heap to upload our texture to the GPU
	hr = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize),
		// resource description for a buffer (storing the image data in this heap just to copy to the default heap)
		D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
		nullptr,
		IID_PPV_ARGS(&textureBufferUploadHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	textureBufferUploadHeap->SetName(L"Texture Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &imageData[0]; // pointer to our image data
	textureData.RowPitch = imageBytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = imageBytesPerRow * textureDesc.Height; // also the size of our triangle vertex data

	// Now we copy the upload buffer contents to the default heap
	UpdateSubresources(m_commandList.Get(), textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// create the descriptor heap that will store our srv
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// heapDesc.NumDescriptors = 2;
	heapDesc.NumDescriptors = 3;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mainDescriptorHeap));
	if (FAILED(hr)) {
		Running = false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 3;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));
	if (FAILED(hr)) {
		Running = false;
	}

	// now we create a shader resource view (descriptor that points to the texture and describes it)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	m_device->CreateShaderResourceView(textureBuffer.Get(), &srvDesc,
	                                   mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto mrRobotFontPath = GetAssetFullPath(L"mr-robot.fnt");
	mrRobotFont = LoadFont(mrRobotFontPath.c_str(), g_ClientWidth, g_ClientHeight);

	D3D12_RESOURCE_DESC fontTextureDesc;
	int fontImageBytesPerRow;
	BYTE* fontImageData;
	int fontImageSize = LoadImageDataFromFile(&fontImageData, fontTextureDesc, mrRobotFont.fontImage.c_str(),
	                                          fontImageBytesPerRow);
	if (fontImageSize <= 0) {
		Running = false;
		return false;
	}

	hr = m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
	                                       &fontTextureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
	                                       IID_PPV_ARGS(&mrRobotFont.textureBuffer));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	mrRobotFont.textureBuffer->SetName(L"Font Texture Buffer Resource Heap");

	wrl::ComPtr<ID3D12Resource> fontTextureBufferUploadHeap;
	UINT64 fontTextureUploadBufferSize;
	m_device->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize);
	hr = m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
	                                       &CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize),
	                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	                                       IID_PPV_ARGS(&fontTextureBufferUploadHeap));
	if (FAILED(hr)) {
		Running = false;
		return false;
	}
	fontTextureBufferUploadHeap->SetName(L"Font Texture Buffer Upload Resource Heap");

	D3D12_SUBRESOURCE_DATA fontTextureData = {};
	fontTextureData.pData = &fontImageData[0];
	fontTextureData.RowPitch = fontImageBytesPerRow;
	fontTextureData.SlicePitch = fontImageBytesPerRow * fontTextureDesc.Height;

	UpdateSubresources(m_commandList.Get(), mrRobotFont.textureBuffer.Get(), fontTextureBufferUploadHeap.Get(), 0, 0, 1,
	                   &fontTextureData);
	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               mrRobotFont.textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// create an srv for the font
	D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
	fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	fontsrvDesc.Format = fontTextureDesc.Format;
	fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	fontsrvDesc.Texture2D.MipLevels = 1;

	// we need to get the next descriptor location in the descriptor heap to store this srv
	srvHandleSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	mrRobotFont.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1,
	                                                      srvHandleSize);

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, srvHandleSize);
	m_device->CreateShaderResourceView(mrRobotFont.textureBuffer.Get(), &fontsrvDesc, srvHandle);

	// create text vertex buffer committed resources

	for (int i = 0; i < FrameCount; ++i) {
		// create upload heap. We will fill this with data for our text
		ID3D12Resource* vBufferUploadHeap;
		hr = m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(maxNumTextCharacters * sizeof(CETextVertex)),
			// resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ,
			// GPU will read from this buffer and copy its contents to the default heap
			nullptr,
			IID_PPV_ARGS(&textVertexBuffer[i]));
		if (FAILED(hr)) {
			Running = false;
			return false;
		}
		textVertexBuffer[i]->SetName(L"Text Vertex Buffer Upload Resource Heap");

		CD3DX12_RANGE readRange(0, 0);
		// We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

		// map the resource heap to get a gpu virtual address to the beginning of the heap
		hr = textVertexBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&textVBGPUAddress[i]));
	}
	// create the text pso

	// Now we execute the command list to upload the initial assets (triangle data)
	m_commandList->Close();
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	m_fenceValues[m_frameIndex]++;
	hr = m_commandQueue->Signal(m_fences[m_frameIndex].Get(), m_fenceValues[m_frameIndex]);
	if (FAILED(hr)) {
		Running = false;
		return false;
	}

	// we are done with image data now that we've uploaded it to the gpu, so free it up
	delete fontImageData;
	delete imageData;

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(CEVertexPosTex);
	m_VertexBufferView.SizeInBytes = vBufferSize;

	for (int i = 0; i < FrameCount; ++i) {
		textVertexBufferView[i].BufferLocation = textVertexBuffer[i]->GetGPUVirtualAddress();
		textVertexBufferView[i].StrideInBytes = sizeof(CETextVertex);
		textVertexBufferView[i].SizeInBytes = maxNumTextCharacters * sizeof(CETextVertex);
	}

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IndexBufferView.SizeInBytes = iBufferSize;

	// Fill out the Viewport
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = g_ClientWidth;
	m_Viewport.Height = g_ClientHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = g_ClientWidth;
	m_ScissorRect.bottom = g_ClientHeight;

	// build projection and view matrix
	XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(45.0f * (3.14f / 180.0f), (float)g_ClientWidth / (float)g_ClientHeight,
	                                           0.1f, 1000.0f);
	XMStoreFloat4x4(&cameraProjMat, tmpMat);

	// set starting camera state
	cameraPosition = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	cameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	// build view matrix
	XMVECTOR cPos = XMLoadFloat4(&cameraPosition);
	XMVECTOR cTarg = XMLoadFloat4(&cameraTarget);
	XMVECTOR cUp = XMLoadFloat4(&cameraUp);
	tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
	XMStoreFloat4x4(&cameraViewMat, tmpMat);

	// set starting cubes position
	// first cube
	cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
	XMVECTOR posVec = XMLoadFloat4(&cube1Position); // create xmvector for cube1's position

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube1's position vector
	XMStoreFloat4x4(&cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube1WorldMat, tmpMat); // store cube1's world matrix

	// second cube
	cube2PositionOffset = XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
	posVec = XMLoadFloat4(&cube2PositionOffset) + XMLoadFloat4(&cube1Position); // create xmvector for cube2's position
	// we are rotating around cube1 here, so add cube2's position to cube1

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube2's position offset vector
	XMStoreFloat4x4(&cube2RotMat, XMMatrixIdentity()); // initialize cube2's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube2WorldMat, tmpMat); // store cube2's world matrix

	return true;
}

void CEDirect3DGraphics::CreateDirect3D11(int width, int height) {
}

void CEDirect3DGraphics::PrintGraphicsVersion() {
	const auto info = GetGPUInfo();
	std::wstringstream wss;
	wss << "GPU VRAM: " << std::endl;
	wss << "AvailableForReservation: " << ID3DGPUInfo(info).dqvmi.AvailableForReservation << std::endl;
	wss << "CurrentUsage: " << ID3DGPUInfo(info).dqvmi.CurrentUsage << std::endl;
	wss << "CurrentReservation: " << ID3DGPUInfo(info).dqvmi.CurrentReservation << std::endl;
	wss << "Budget: " << ID3DGPUInfo(info).dqvmi.Budget << std::endl;
	OutputDebugStringW(wss.str().c_str());
}

void CEDirect3DGraphics::WaitForPreviousFrame() {

	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (m_fences[m_frameIndex]->GetCompletedValue() < m_fenceValues[m_frameIndex]) {
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		hr = m_fences[m_frameIndex]->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
		if (FAILED(hr)) {
			Running = false;
		}

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// increment fenceValue for next frame
	m_fenceValues[m_frameIndex]++;
}
