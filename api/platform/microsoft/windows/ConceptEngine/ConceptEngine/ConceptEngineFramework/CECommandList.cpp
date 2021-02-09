#include "CECommandList.h"

#include "CEGenerateMipsPSO.h"
#include "CECommandQueue.h"
#include "CEConstantBuffer.h"
#include "CEDX12Libs.h"
#include "CEDevice.h"
#include "CEDynamicDescriptorHeap.h"
#include "CEHelper.h"
#include "CEIndexBuffer.h"
#include "CEMaterial.h"
#include "CEMesh.h"
#include "CEPanoToCubemapPSO.h"
#include "CEPipelineStateObject.h"
#include "CEResource.h"
#include "CEResourceStateTracker.h"
#include "CERootSignature.h"
#include "CEScene.h"
#include "CESceneNode.h"
#include "CETexture.h"
#include "CEUploadBuffer.h"
#include "CEVertexBuffer.h"
#include "CEBuffer.h"
#include "CEConstantBufferView.h"
#include "CEDirect3DGraphics.h"
#include "CERenderTarget.h"
#include "CEShaderResourceView.h"
#include "CEUnorderedAccessView.h"
#include <wincodec.h>


#include "CEDXILLibrary.h"
#include "CESwapChain.h"
#include "RaytracingSceneDefines.h"


using namespace Concept::GraphicsEngine::Direct3D;

class CEUploadBufferInstance : public CEUploadBuffer {

public:
	CEUploadBufferInstance(CEDevice& device, size_t pageSize = _2MB)
		: CEUploadBuffer(device, pageSize) {
	}

	virtual ~CEUploadBufferInstance() {

	};
};

std::map<std::wstring, ID3D12Resource*> CECommandList::m_textureCache;
std::mutex CECommandList::m_textureCacheMutex;

CECommandList::CECommandList(CEDevice& device, D3D12_COMMAND_LIST_TYPE type)
	: m_device(device)
	  , m_commandListType(type)
	  , m_rootSignature(nullptr)
	  , m_pipelineState(nullptr) {
	auto d3d12Device = m_device.GetDevice();

	ThrowIfFailed(
		d3d12Device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&m_commandAllocator)));

	ThrowIfFailed(d3d12Device->CreateCommandList(0, m_commandListType, m_commandAllocator.Get(), nullptr,
	                                             IID_PPV_ARGS(&m_commandList)));

	m_uploadBuffer = std::make_unique<CEUploadBufferInstance>(device);

	m_resourceStateTracker = std::make_unique<CEResourceStateTracker>();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i] = std::make_unique<CEDynamicDescriptorHeap>(
			device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		m_descriptorHeaps[i] = nullptr;
	}

	if (m_device.GetRayTracingTier() != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
		CreateRayTracingCommandList();
	}
}

CECommandList::~CECommandList() {
}

void CECommandList::CreateRayTracingCommandList() {
	ThrowIfFailed(m_commandList->QueryInterface(IID_PPV_ARGS(&m_rtxCommandList)));
}

void CECommandList::TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter,
                                      UINT subresource, bool flushBarriers) {
	if (resource) {
		// The "before" state is not important. It will be resolved by the resource state tracker.
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, stateAfter,
		                                                    subresource);
		m_resourceStateTracker->ResourceBarrier(barrier);
	}

	if (flushBarriers) {
		FlushResourceBarriers();
	}
}

void CECommandList::TransitionBarrier(const std::shared_ptr<CEResource>& resource, D3D12_RESOURCE_STATES stateAfter,
                                      UINT subresource, bool flushBarriers) {
	if (resource) {
		TransitionBarrier(resource->GetD3D12Resource(), stateAfter, subresource, flushBarriers);
	}
}

void CECommandList::UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, bool flushBarriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());

	m_resourceStateTracker->ResourceBarrier(barrier);

	if (flushBarriers) {
		FlushResourceBarriers();
	}
}

void CECommandList::UAVBarrier(const std::shared_ptr<CEResource>& resource, bool flushBarriers) {
	auto d3d12Resource = resource ? resource->GetD3D12Resource() : nullptr;
	UAVBarrier(d3d12Resource, flushBarriers);
}

void CECommandList::AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> beforeResource,
                                    Microsoft::WRL::ComPtr<ID3D12Resource> afterResource, bool flushBarriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(beforeResource.Get(), afterResource.Get());

	m_resourceStateTracker->ResourceBarrier(barrier);

	if (flushBarriers) {
		FlushResourceBarriers();
	}
}

void CECommandList::AliasingBarrier(const std::shared_ptr<CEResource>& beforeResource,
                                    const std::shared_ptr<CEResource>& afterResource, bool flushBarriers) {
	auto d3d12BeforeResource = beforeResource ? beforeResource->GetD3D12Resource() : nullptr;
	auto d3d12AfterResource = afterResource ? afterResource->GetD3D12Resource() : nullptr;

	AliasingBarrier(d3d12BeforeResource, d3d12AfterResource, flushBarriers);
}

void CECommandList::FlushResourceBarriers() {
	m_resourceStateTracker->FlushResourceBarriers(shared_from_this());
}

void CECommandList::CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> dstRes,
                                 Microsoft::WRL::ComPtr<ID3D12Resource> srcRes) {
	assert(dstRes);
	assert(srcRes);

	TransitionBarrier(dstRes, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(srcRes, D3D12_RESOURCE_STATE_COPY_SOURCE);

	FlushResourceBarriers();

	m_commandList->CopyResource(dstRes.Get(), srcRes.Get());

	TrackResource(dstRes);
	TrackResource(srcRes);
}

void CECommandList::CopyResource(const std::shared_ptr<CEResource>& dstRes, const std::shared_ptr<CEResource>& srcRes) {
	assert(dstRes && srcRes);

	CopyResource(dstRes->GetD3D12Resource(), srcRes->GetD3D12Resource());
}

void CECommandList::ResolveSubResource(const std::shared_ptr<CEResource>& dstRes,
                                       const std::shared_ptr<CEResource>& srcRes,
                                       uint32_t dstSubresource, uint32_t srcSubresource) {
	assert(dstRes && srcRes);

	TransitionBarrier(dstRes, D3D12_RESOURCE_STATE_RESOLVE_DEST, dstSubresource);
	TransitionBarrier(srcRes, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, srcSubresource);

	FlushResourceBarriers();

	m_commandList->ResolveSubresource(dstRes->GetD3D12Resource().Get(), dstSubresource,
	                                  srcRes->GetD3D12Resource().Get(), srcSubresource,
	                                  dstRes->GetD3D12ResourceDesc().Format);

	TrackResource(srcRes);
	TrackResource(dstRes);
}

ComPtr<ID3D12Resource>
CECommandList::CopyBuffer(size_t bufferSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags) {
	ComPtr<ID3D12Resource> d3d12Resource;
	if (bufferSize == 0) {
		// This will result in a NULL resource (which may be desired to define a default null resource).
	}
	else {
		auto d3d12Device = m_device.GetDevice();

		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags), D3D12_RESOURCE_STATE_COMMON, nullptr,
			IID_PPV_ARGS(&d3d12Resource)));

		// Add the resource to the global resource state tracker.
		CEResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		if (bufferData != nullptr) {
			// Create an upload resource to use as an intermediate buffer to copy the buffer resource
			ComPtr<ID3D12Resource> uploadResource;
			ThrowIfFailed(d3d12Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&uploadResource)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			m_resourceStateTracker->TransitionResource(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();

			UpdateSubresources(m_commandList.Get(), d3d12Resource.Get(), uploadResource.Get(), 0, 0, 1,
			                   &subresourceData);

			// Add references to resources so they stay in scope until the command list is reset.
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	}

	return d3d12Resource;
}

std::shared_ptr<CEVertexBuffer> CECommandList::CopyVertexBuffer(size_t numVertices, size_t vertexStride,
                                                                const void* vertexBufferData) {
	auto d3d12Resource = CopyBuffer(numVertices * vertexStride, vertexBufferData);
	std::shared_ptr<CEVertexBuffer> vertexBuffer =
		m_device.CreateVertexBuffer(d3d12Resource, numVertices, vertexStride);

	return vertexBuffer;
}

std::shared_ptr<CEIndexBuffer> CECommandList::CopyIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat,
                                                              const void* indexBufferData) {
	size_t elementSize = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;

	auto d3d12Resource = CopyBuffer(numIndicies * elementSize, indexBufferData);

	std::shared_ptr<CEIndexBuffer> indexBuffer = m_device.CreateIndexBuffer(d3d12Resource, numIndicies, indexFormat);

	return indexBuffer;
}

std::shared_ptr<CEConstantBuffer> CECommandList::CopyConstantBuffer(size_t bufferSize, const void* bufferData) {
	auto d3d12Resource = CopyBuffer(bufferSize, bufferData);

	std::shared_ptr<CEConstantBuffer> constantBuffer = m_device.CreateConstantBuffer(d3d12Resource);

	return constantBuffer;
}

std::shared_ptr<CEByteAddressBuffer> CECommandList::CopyByteAddressBuffer(size_t bufferSize, const void* bufferData) {
	auto d3d12Resource = CopyBuffer(bufferSize, bufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	std::shared_ptr<CEByteAddressBuffer> byteAddressBuffer = m_device.CreateByteAddressBuffer(d3d12Resource);

	return byteAddressBuffer;
}


std::shared_ptr<CEStructuredBuffer> CECommandList::CopyStructuredBuffer(size_t numElements, size_t elementSize,
                                                                        const void* bufferData) {
	auto d3d12Resource =
		CopyBuffer(numElements * elementSize, bufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	std::shared_ptr<CEStructuredBuffer> structuredBuffer =
		m_device.CreateStructuredBuffer(d3d12Resource, numElements, elementSize);

	return structuredBuffer;
}

void CECommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology) {
	m_commandList->IASetPrimitiveTopology(primitiveTopology);
}


int CECommandList::LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription,
                                         LPCWSTR filename, int& bytesPerRow) {
	HRESULT hr;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory* wicFactory;

	// reset decoder, frame, and converter, since these will be different for each image we load
	IWICBitmapDecoder* wicDecoder = NULL;
	IWICBitmapFrameDecode* wicFrame = NULL;
	IWICFormatConverter* wicConverter = NULL;

	bool imageConverted = false;

	// if (wicFactory == NULL) {
	// Initialize the COM library
	// CoInitialize(NULL);

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
	// }

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

std::shared_ptr<CETexture> CECommandList::LoadTextureFromFile(const std::wstring& fileName, bool sRGB) {
	std::shared_ptr<CETexture> texture;
	fs::path filePath(fileName);
	if (!fs::exists(filePath)) {
		throw std::exception("File not found.");
	}

	std::lock_guard<std::mutex> lock(m_textureCacheMutex);
	auto iter = m_textureCache.find(fileName);
	if (iter != m_textureCache.end()) {
		texture = m_device.CreateTexture(iter->second);
	}
	else {
		TexMetadata metadata;
		ScratchImage scratchImage;

		if (filePath.extension() == ".dds") {
			ThrowIfFailed(LoadFromDDSFile(fileName.c_str(), DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}
		else if (filePath.extension() == ".hdr") {
			ThrowIfFailed(LoadFromHDRFile(fileName.c_str(), &metadata, scratchImage));
		}
		else if (filePath.extension() == ".tga") {
			ThrowIfFailed(LoadFromTGAFile(fileName.c_str(), &metadata, scratchImage));
		}
		else {
			ThrowIfFailed(LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}

		// Force the texture format to be sRGB to convert to linear when sampling the texture in a shader.
		if (sRGB) {
			metadata.format = MakeSRGB(metadata.format);
		}

		D3D12_RESOURCE_DESC textureDesc = {};
		switch (metadata.dimension) {
		case TEX_DIMENSION_TEXTURE1D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(metadata.format, static_cast<UINT64>(metadata.width),
			                                           static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE2D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<UINT64>(metadata.width),
			                                           static_cast<UINT>(metadata.height),
			                                           static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE3D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(metadata.format, static_cast<UINT64>(metadata.width),
			                                           static_cast<UINT>(metadata.height),
			                                           static_cast<UINT16>(metadata.depth));
			break;
		default:
			throw std::exception("Invalid texture dimension.");
			break;
		}

		auto d3d12Device = m_device.GetDevice();
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc,
			D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&textureResource)));

		texture = m_device.CreateTexture(textureResource);
		texture->SetName(fileName);

		// Update the global state tracker.
		CEResourceStateTracker::AddGlobalResourceState(textureResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
		const Image* pImages = scratchImage.GetImages();
		for (int i = 0; i < scratchImage.GetImageCount(); ++i) {
			auto& subresource = subresources[i];
			subresource.RowPitch = pImages[i].rowPitch;
			subresource.SlicePitch = pImages[i].slicePitch;
			subresource.pData = pImages[i].pixels;
		}

		CopyTextureSubResource(texture, 0, static_cast<uint32_t>(subresources.size()), subresources.data());

		if (subresources.size() < textureResource->GetDesc().MipLevels) {
			GenerateMips(texture);
		}

		// Add the texture resource to the texture cache.
		m_textureCache[fileName] = textureResource.Get();
	}

	return texture;
}

void CECommandList::GenerateMips(const std::shared_ptr<CETexture>& texture) {
	if (!texture)
		return;

	auto d3d12Device = m_device.GetDevice();

	if (m_commandListType == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_computeCommandList) {
			m_computeCommandList = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}
		m_computeCommandList->GenerateMips(texture);
		return;
	}

	auto d3d12Resource = texture->GetD3D12Resource();

	// If the texture doesn't have a valid resource? Do nothing...
	if (!d3d12Resource)
		return;
	auto resourceDesc = d3d12Resource->GetDesc();

	// If the texture only has a single mip level (level 0)
	// do nothing.
	if (resourceDesc.MipLevels == 1)
		return;
	// Currently, only non-multi-sampled 2D textures are supported.
	if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || resourceDesc.DepthOrArraySize != 1 ||
		resourceDesc.SampleDesc.Count > 1) {
		throw std::exception("GenerateMips is only supported for non-multi-sampled 2D Textures.");
	}

	ComPtr<ID3D12Resource> uavResource = d3d12Resource;
	// Create an alias of the original resource.
	// This is done to perform a GPU copy of resources with different formats.
	// BGR -> RGB texture copies will fail GPU validation unless performed
	// through an alias of the BRG resource in a placed heap.
	ComPtr<ID3D12Resource> aliasResource;

	// If the passed-in resource does not allow for UAV access
	// then create a staging resource that is used to generate
	// the mipmap chain.
	if (!texture->CheckUAVSupport() || (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		// Describe an alias resource that is used to copy the original texture.
		auto aliasDesc = resourceDesc;
		// Placed resources can't be render targets or depth-stencil views.
		aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		// Describe a UAV compatible resource that is used to perform
		// mipmapping of the original texture.
		auto uavDesc = aliasDesc; // The flags for the UAV description must match that of the alias description.
		uavDesc.Format = CETexture::GetUAVCompatableFormat(resourceDesc.Format);

		D3D12_RESOURCE_DESC resourceDescs[] = {aliasDesc, uavDesc};

		// Create a heap that is large enough to store a copy of the original resource.
		auto allocationInfo = d3d12Device->GetResourceAllocationInfo(0, _countof(resourceDescs), resourceDescs);

		D3D12_HEAP_DESC heapDesc = {};
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Alignment = allocationInfo.Alignment;
		heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

		ComPtr<ID3D12Heap> heap;
		ThrowIfFailed(d3d12Device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));

		// Make sure the heap does not go out of scope until the command list
		// is finished executing on the command queue.
		TrackResource(heap);

		// Create a placed resource that matches the description of the
		// original resource. This resource is used to copy the original
		// texture to the UAV compatible resource.
		ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(), 0, &aliasDesc, D3D12_RESOURCE_STATE_COMMON,
		                                                nullptr, IID_PPV_ARGS(&aliasResource)));

		CEResourceStateTracker::AddGlobalResourceState(aliasResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		// Ensure the scope of the alias resource.
		TrackResource(aliasResource);

		// Create a UAV compatible resource in the same heap as the alias
		// resource.
		ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(), 0, &uavDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
		                                                IID_PPV_ARGS(&uavResource)));

		CEResourceStateTracker::AddGlobalResourceState(uavResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		// Ensure the scope of the UAV compatible resource.
		TrackResource(uavResource);

		// Add an aliasing barrier for the alias resource.
		AliasingBarrier(nullptr, aliasResource);

		// Copy the original resource to the alias resource.
		// This ensures GPU validation.
		CopyResource(aliasResource, d3d12Resource);

		// Add an aliasing barrier for the UAV compatible resource.
		AliasingBarrier(aliasResource, uavResource);
	}

	// Generate mips with the UAV compatible resource.
	auto uavTexture = m_device.CreateTexture(uavResource);
	GenerateMipsUAV(uavTexture, CETexture::IsSRGBFormat(resourceDesc.Format));

	if (aliasResource) {
		AliasingBarrier(uavResource, aliasResource);
		// Copy the alias resource back to the original resource.
		CopyResource(d3d12Resource, aliasResource);
	}
}

void CECommandList::GenerateMipsUAV(const std::shared_ptr<CETexture>& texture, bool isSRGB) {
	if (!m_generateMipsPSO) {
		m_generateMipsPSO = std::make_unique<CEGenerateMipsPSO>(m_device);
	}

	SetPipelineState(m_generateMipsPSO->GetPipelineState());
	SetComputeRootSignature(m_generateMipsPSO->GetRootSignature());

	GenerateMipsCB generateMipsCB;
	generateMipsCB.IsSRGB = isSRGB;

	auto resource = texture->GetD3D12Resource();
	auto resourceDesc = resource->GetDesc();

	// Create an SRV that uses the format of the original texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = isSRGB ? CETexture::GetSRGBFormat(resourceDesc.Format) : resourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension =
		D3D12_SRV_DIMENSION_TEXTURE2D; // Only 2D textures are supported (this was checked in the calling function).
	srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	auto srv = m_device.CreateShaderResourceView(texture, &srvDesc);

	for (uint32_t srcMip = 0; srcMip < resourceDesc.MipLevels - 1u;) {
		uint64_t srcWidth = resourceDesc.Width >> srcMip;
		uint32_t srcHeight = resourceDesc.Height >> srcMip;
		uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
		uint32_t dstHeight = srcHeight >> 1;

		// 0b00(0): Both width and height are even.
		// 0b01(1): Width is odd, height is even.
		// 0b10(2): Width is even, height is odd.
		// 0b11(3): Both width and height are odd.
		generateMipsCB.SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);

		// How many mipmap levels to compute this pass (max 4 mips per pass)
		DWORD mipCount;

		// The number of times we can half the size of the texture and get
		// exactly a 50% reduction in size.
		// A 1 bit in the width or height indicates an odd dimension.
		// The case where either the width or the height is exactly 1 is handled
		// as a special case (as the dimension does not require reduction).
		_BitScanForward(&mipCount,
		                (dstWidth == 1 ? dstHeight : dstWidth) | (dstHeight == 1 ? dstWidth : dstHeight));
		// Maximum number of mips to generate is 4.
		mipCount = std::min<DWORD>(4, mipCount + 1);
		// Clamp to total number of mips left over.
		mipCount = (srcMip + mipCount) >= resourceDesc.MipLevels ? resourceDesc.MipLevels - srcMip - 1 : mipCount;

		// Dimensions should not reduce to 0.
		// This can happen if the width and height are not the same.
		dstWidth = std::max<DWORD>(1, dstWidth);
		dstHeight = std::max<DWORD>(1, dstHeight);

		generateMipsCB.SrcMipLevel = srcMip;
		generateMipsCB.NumMipLevels = mipCount;
		generateMipsCB.TexelSize.x = 1.0f / (float)dstWidth;
		generateMipsCB.TexelSize.y = 1.0f / (float)dstHeight;

		SetCompute32BitConstants(GenerateMips::GenerateMipsCB, generateMipsCB);

		SetShaderResourceView(GenerateMips::SrcMip, 0, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, srcMip,
		                      1);

		for (uint32_t mip = 0; mip < mipCount; ++mip) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = srcMip + mip + 1;

			auto uav = m_device.CreateUnorderedAccessView(texture, nullptr, &uavDesc);
			SetUnorderedAccessView(GenerateMips::OutMip, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			                       srcMip + mip + 1, 1);
		}

		// Pad any unused mip levels with a default UAV. Doing this keeps the DX12 runtime happy.
		if (mipCount < 4) {
			m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
				GenerateMips::OutMip, mipCount, 4 - mipCount, m_generateMipsPSO->GetDefaultUAV());
		}

		Dispatch(Math::DivideByMultiple(dstWidth, 8), Math::DivideByMultiple(dstHeight, 8));

		UAVBarrier(texture);

		srcMip += mipCount;
	}
}

void CECommandList::PanoToCubeMap(const std::shared_ptr<CETexture>& cubemapTexture,
                                  const std::shared_ptr<CETexture>& panoTexture) {
	assert(cubemapTexture && panoTexture);

	if (m_commandListType == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_computeCommandList) {
			m_computeCommandList = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}
		m_computeCommandList->PanoToCubeMap(cubemapTexture, panoTexture);
		return;
	}

	if (!m_panoToCubemapPSO) {
		m_panoToCubemapPSO = std::make_unique<CEPanoToCubemapPSO>(m_device);
	}

	auto cubemapResource = cubemapTexture->GetD3D12Resource();
	if (!cubemapResource)
		return;

	CD3DX12_RESOURCE_DESC cubemapDesc(cubemapResource->GetDesc());

	auto stagingResource = cubemapResource;
	auto stagingTexture = m_device.CreateTexture(stagingResource);
	// If the passed-in resource does not allow for UAV access
	// then create a staging resource that is used to generate
	// the cubemap.
	if ((cubemapDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		auto d3d12Device = m_device.GetDevice();

		auto stagingDesc = cubemapDesc;
		stagingDesc.Format = CETexture::GetUAVCompatableFormat(cubemapDesc.Format);
		stagingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &stagingDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&stagingResource)

		));

		CEResourceStateTracker::AddGlobalResourceState(stagingResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);

		stagingTexture = m_device.CreateTexture(stagingResource);
		stagingTexture->SetName(L"Pano to Cubemap Staging Texture");

		CopyResource(stagingTexture, cubemapTexture);
	}

	TransitionBarrier(stagingTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	SetPipelineState(m_panoToCubemapPSO->GetPipelineState());
	SetComputeRootSignature(m_panoToCubemapPSO->GetRootSignature());

	PanoToCubemapCB panoToCubemapCB;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = CETexture::GetUAVCompatableFormat(cubemapDesc.Format);
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.ArraySize = 6;

	auto srv = m_device.CreateShaderResourceView(panoTexture);
	SetShaderResourceView(PanoToCubemapRS::SrcTexture, 0, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	for (uint32_t mipSlice = 0; mipSlice < cubemapDesc.MipLevels;) {
		// Maximum number of mips to generate per pass is 5.
		uint32_t numMips = std::min<uint32_t>(5, cubemapDesc.MipLevels - mipSlice);

		panoToCubemapCB.FirstMip = mipSlice;
		panoToCubemapCB.CubemapSize =
			std::max<uint32_t>(static_cast<uint32_t>(cubemapDesc.Width), cubemapDesc.Height) >> mipSlice;
		panoToCubemapCB.NumMips = numMips;

		SetCompute32BitConstants(PanoToCubemapRS::PanoToCubemapCB, panoToCubemapCB);

		for (uint32_t mip = 0; mip < numMips; ++mip) {
			uavDesc.Texture2DArray.MipSlice = mipSlice + mip;

			auto uav = m_device.CreateUnorderedAccessView(stagingTexture, nullptr, &uavDesc);
			SetUnorderedAccessView(PanoToCubemapRS::DstMips, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 0, 0);
		}

		if (numMips < 5) {
			// Pad unused mips. This keeps DX12 runtime happy.
			m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
				PanoToCubemapRS::DstMips, panoToCubemapCB.NumMips, 5 - numMips, m_panoToCubemapPSO->GetDefaultUAV());
		}

		Dispatch(Math::DivideByMultiple(panoToCubemapCB.CubemapSize, 16),
		         Math::DivideByMultiple(panoToCubemapCB.CubemapSize, 16), 6);

		mipSlice += numMips;
	}

	if (stagingResource != cubemapResource) {
		CopyResource(cubemapTexture, stagingTexture);
	}
}

std::shared_ptr<CEScene> CECommandList::LoadSceneFromFile(const std::wstring& fileName,
                                                          const std::function<bool(float)>& loadingProgress) {
	auto scene = std::make_shared<CEScene>();

	if (scene->LoadSceneFromFile(*this, fileName, loadingProgress)) {
		return scene;
	}

	return nullptr;
}

std::shared_ptr<CEScene> CECommandList::LoadSceneFromString(const std::string& sceneString, const std::string& format) {
	auto scene = std::make_shared<CEScene>();

	scene->LoadSceneFromString(*this, sceneString, format);

	return scene;
}

// Helper function to create a Scene from an index and vertex buffer.
std::shared_ptr<CEScene> CECommandList::CreateScene(const VertexCollection& vertices, const IndexCollection& indices,
                                                    std::string sceneName) {
	if (vertices.empty()) {
		return nullptr;
	}

	auto vertexBuffer = CopyVertexBuffer(vertices);
	auto indexBuffer = CopyIndexBuffer(indices);

	auto mesh = std::make_shared<CEMesh>();
	// Create a default white material for new meshes.
	auto material = std::make_shared<CEMaterial>(CEMaterial::White);

	mesh->SetVertexBuffer(0, vertexBuffer);
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetMaterial(material);


	auto node = std::make_shared<CESceneNode>();
	node->AddMesh(mesh);

	auto scene = std::make_shared<CEScene>();
	scene->SetRootNode(node);

	return scene;
}

std::shared_ptr<CEScene> CECommandList::CreateCube(float size, bool reverseWinding) {
	// Cube is centered at 0,0,0.
	float s = size * 0.5f;

	// 8 edges of cube.
	XMFLOAT3 p[8] = {
		{s, s, -s}, {s, s, s}, {s, -s, s}, {s, -s, -s},
		{-s, s, s}, {-s, s, -s}, {-s, -s, -s}, {-s, -s, s}
	};
	// 6 face normals
	XMFLOAT3 n[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
	// 4 unique texture coordinates
	XMFLOAT3 t[4] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};

	// Indices for the vertex positions.
	uint16_t i[24] = {
		0, 1, 2, 3, // +X
		4, 5, 6, 7, // -X
		4, 1, 0, 5, // +Y
		2, 7, 6, 3, // -Y
		1, 4, 7, 2, // +Z
		5, 0, 3, 6 // -Z
	};

	VertexCollection vertices;
	IndexCollection indices;

	for (uint16_t f = 0; f < 6; ++f) // For each face of the cube.
	{
		// Four vertices per face.
		vertices.emplace_back(p[i[f * 4 + 0]], n[f], t[0]);
		vertices.emplace_back(p[i[f * 4 + 1]], n[f], t[1]);
		vertices.emplace_back(p[i[f * 4 + 2]], n[f], t[2]);
		vertices.emplace_back(p[i[f * 4 + 3]], n[f], t[3]);

		// First triangle.
		indices.emplace_back(f * 4 + 0);
		indices.emplace_back(f * 4 + 1);
		indices.emplace_back(f * 4 + 2);

		// Second triangle
		indices.emplace_back(f * 4 + 2);
		indices.emplace_back(f * 4 + 3);
		indices.emplace_back(f * 4 + 0);
	}

	if (reverseWinding) {
		ReverseWinding(indices, vertices);
	}

	return CreateScene(vertices, indices, "Cube");
}

std::shared_ptr<CEScene> CECommandList::CreateSphere(float radius, uint32_t tessellation, bool reversWinding) {

	if (tessellation < 3)
		throw std::out_of_range("tessellation parameter out of range");

	VertexCollection vertices;
	IndexCollection indices;

	size_t verticalSegments = tessellation;
	size_t horizontalSegments = tessellation * 2;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= verticalSegments; i++) {
		float v = 1 - (float)i / verticalSegments;

		float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
		float dy, dxz;

		DirectX::XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= horizontalSegments; j++) {
			float u = (float)j / horizontalSegments;

			float longitude = j * XM_2PI / horizontalSegments;
			float dx, dz;

			DirectX::XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			auto normal = XMVectorSet(dx, dy, dz, 0);
			auto textureCoordinate = XMVectorSet(u, v, 0, 0);
			auto position = normal * radius;

			vertices.emplace_back(position, normal, textureCoordinate);
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = horizontalSegments + 1;

	for (size_t i = 0; i < verticalSegments; i++) {
		for (size_t j = 0; j <= horizontalSegments; j++) {
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			indices.push_back(i * stride + nextJ);
			indices.push_back(nextI * stride + j);
			indices.push_back(i * stride + j);

			indices.push_back(nextI * stride + nextJ);
			indices.push_back(nextI * stride + j);
			indices.push_back(i * stride + nextJ);
		}
	}

	if (reversWinding) {
		ReverseWinding(indices, vertices);
	}

	return CreateScene(vertices, indices, "Sphere");
}

void CECommandList::CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation,
                                      float height, float radius, bool isTop) {
	// Create cap indices.
	for (size_t i = 0; i < tessellation - 2; i++) {
		size_t i1 = (i + 1) % tessellation;
		size_t i2 = (i + 2) % tessellation;

		if (isTop) {
			std::swap(i1, i2);
		}

		size_t vbase = vertices.size();
		indices.push_back(vbase + i2);
		indices.push_back(vbase + i1);
		indices.push_back(vbase);
	}

	// Which end of the cylinder is this?
	XMVECTOR normal = g_XMIdentityR1;
	XMVECTOR textureScale = g_XMNegativeOneHalf;

	if (!isTop) {
		normal = XMVectorNegate(normal);
		textureScale = XMVectorMultiply(textureScale, g_XMNegateX);
	}

	// Create cap vertices.
	for (size_t i = 0; i < tessellation; i++) {
		XMVECTOR circleVector = GetCircleVector(i, tessellation);
		XMVECTOR position = XMVectorAdd(XMVectorScale(circleVector, radius), XMVectorScale(normal, height));
		XMVECTOR textureCoordinate =
			XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circleVector), textureScale, g_XMOneHalf);

		vertices.emplace_back(position, normal, textureCoordinate);
	}
}

std::shared_ptr<CEScene> CECommandList::CreateCylinder(float radius, float height, uint32_t tessellation,
                                                       bool reverseWinding) {
	if (tessellation < 3)
		throw std::out_of_range("tessellation parameter out of range");

	VertexCollection vertices;
	IndexCollection indices;

	height /= 2;

	XMVECTOR topOffset = XMVectorScale(g_XMIdentityR1, height);

	size_t stride = tessellation + 1;

	// Create a ring of triangles around the outside of the cylinder.
	for (size_t i = 0; i <= tessellation; i++) {
		XMVECTOR normal = GetCircleVector(i, tessellation);

		XMVECTOR sideOffset = XMVectorScale(normal, radius);

		float u = float(i) / float(tessellation);

		XMVECTOR textureCoordinate = XMLoadFloat(&u);

		vertices.emplace_back(XMVectorAdd(sideOffset, topOffset), normal, textureCoordinate);
		vertices.emplace_back(XMVectorSubtract(sideOffset, topOffset), normal,
		                      XMVectorAdd(textureCoordinate, g_XMIdentityR1));

		indices.push_back(i * 2 + 1);
		indices.push_back((i * 2 + 2) % (stride * 2));
		indices.push_back(i * 2);

		indices.push_back((i * 2 + 3) % (stride * 2));
		indices.push_back((i * 2 + 2) % (stride * 2));
		indices.push_back(i * 2 + 1);
	}

	// Create flat triangle fan caps to seal the top and bottom.
	CreateCylinderCap(vertices, indices, tessellation, height, radius, true);
	CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

	// Build RH above
	if (reverseWinding) {
		ReverseWinding(indices, vertices);
	}

	return CreateScene(vertices, indices, "Cylinder");
}

std::shared_ptr<CEScene> CECommandList::CreateCone(float radius, float height, uint32_t tessellation,
                                                   bool reverseWinding) {
	if (tessellation < 3)
		throw std::out_of_range("tessellation parameter out of range");

	VertexCollection vertices;
	IndexCollection indices;

	height /= 2;

	XMVECTOR topOffset = XMVectorScale(g_XMIdentityR1, height);

	size_t stride = tessellation + 1;

	// Create a ring of triangles around the outside of the cone.
	for (size_t i = 0; i <= tessellation; i++) {
		XMVECTOR circlevec = GetCircleVector(i, tessellation);

		XMVECTOR sideOffset = XMVectorScale(circlevec, radius);

		float u = float(i) / float(tessellation);

		XMVECTOR textureCoordinate = XMLoadFloat(&u);

		XMVECTOR pt = XMVectorSubtract(sideOffset, topOffset);

		XMVECTOR normal = XMVector3Cross(GetCircleTangent(i, tessellation), XMVectorSubtract(topOffset, pt));
		normal = XMVector3Normalize(normal);

		// Duplicate the top vertex for distinct normals
		vertices.emplace_back(topOffset, normal, g_XMZero);
		vertices.emplace_back(pt, normal, XMVectorAdd(textureCoordinate, g_XMIdentityR1));

		indices.push_back((i * 2 + 1) % (stride * 2));
		indices.push_back((i * 2 + 3) % (stride * 2));
		indices.push_back(i * 2);
	}

	// Create flat triangle fan caps to seal the bottom.
	CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

	// Build RH above
	if (reverseWinding) {
		ReverseWinding(indices, vertices);
	}

	return CreateScene(vertices, indices, "Cone");
}

std::shared_ptr<CEScene> CECommandList::CreateTorus(float radius, float thickness, uint32_t tessellation,
                                                    bool reverseWinding) {
	assert(tessellation > 3);

	VertexCollection verticies;
	IndexCollection indices;

	size_t stride = tessellation + 1;

	// First we loop around the main ring of the torus.
	for (size_t i = 0; i <= tessellation; i++) {
		float u = (float)i / tessellation;

		float outerAngle = i * XM_2PI / tessellation - XM_PIDIV2;

		// Create a transform matrix that will align geometry to
		// slice perpendicularly though the current ring position.

		XMMATRIX transform = XMMatrixTranslation(radius, 0, 0) * XMMatrixRotationY(outerAngle);

		// Now we loop along the other axis, around the side of the tube.
		for (size_t j = 0; j <= tessellation; j++) {
			float v = 1 - (float)j / tessellation;

			float innerAngle = j * XM_2PI / tessellation + XM_PI;
			float dx, dy;

			XMScalarSinCos(&dy, &dx, innerAngle);

			// Create a vertex.
			auto normal = XMVectorSet(dx, dy, 0, 0);
			auto position = normal * thickness / 2;
			auto textureCoordinate = XMVectorSet(u, v, 0, 0);

			position = XMVector3Transform(position, transform);
			normal = XMVector3TransformNormal(normal, transform);

			verticies.emplace_back(position, normal, textureCoordinate);

			// And create indices for two triangles.
			size_t nextI = (i + 1) % stride;
			size_t nextJ = (j + 1) % stride;

			indices.push_back(nextI * stride + j);
			indices.push_back(i * stride + nextJ);
			indices.push_back(i * stride + j);

			indices.push_back(nextI * stride + j);
			indices.push_back(nextI * stride + nextJ);
			indices.push_back(i * stride + nextJ);
		}
	}

	if (reverseWinding) {
		ReverseWinding(indices, verticies);
	}

	return CreateScene(verticies, indices, "Torus");
}

std::shared_ptr<CEScene> CECommandList::CreatePlane(float width, float height, bool reverseWinding) {
	using Vertex = CEVertexPositionNormalTangentBitangentTexture;

	// clang-format off
	// Define a plane that is aligned with the X-Z plane and the normal is facing up in the Y-axis.
	VertexCollection vertices = {
		Vertex(XMFLOAT3(-0.5f * width, 0.0f, 0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)),
		// 0
		Vertex(XMFLOAT3(0.5f * width, 0.0f, 0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)),
		// 1
		Vertex(XMFLOAT3(0.5f * width, 0.0f, -0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)),
		// 2
		Vertex(XMFLOAT3(-0.5f * width, 0.0f, -0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f),
		       XMFLOAT3(0.0f, 1.0f, 0.0f)) // 3
	};
	// clang-format on
	IndexCollection indices = {1, 3, 0, 2, 3, 1};

	if (reverseWinding) {
		ReverseWinding(indices, vertices);
	}

	return CreateScene(vertices, indices, "Plane");
}

void CECommandList::CreateAccelerationStructures(std::shared_ptr<CEScene>& scene) {
	if (m_device.GetRayTracingTier() != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
		scene->LoadAccelerationStructures(*this);
	}
}

AccelerationStructureBuffers CECommandList::CreateBottomLevelAccelerationStructure(std::shared_ptr<CEMesh> mesh) {
	auto rtxDevice = m_device.GetDevice();
	wrl::ComPtr<ID3D12GraphicsCommandList5> rtxCommandList;
	std::shared_ptr<CECommandList> commandList;
	if (GetCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT) {
		auto& commandQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		commandList = commandQueue.GetCommandList();
		rtxCommandList = commandList->GetCommandList();
	}
	else {
		rtxCommandList = commandList->GetCommandList();
	}


	auto vertexBuffers = mesh->GetVertexBuffers();
	auto indexBuffer = mesh->GetIndexBuffer();
	auto geometryDescSize = (buildMode == ONLY_ONE_BLAS) ? 1 : vertexBuffers.size();

	std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDesc;
	geometryDesc.resize(geometryDescSize);

	//Describe geometry that goes in bottom acceleration structures
	for (uint32_t i = 0; i < geometryDescSize; i++) {
		const auto vertexBuffer = mesh->GetVertexBuffer(i);
		if (vertexBuffer != nullptr) {
			geometryDesc[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geometryDesc[i].Triangles.VertexBuffer.StartAddress = vertexBuffer->GetD3D12Resource()->
				GetGPUVirtualAddress();
			geometryDesc[i].Triangles.VertexBuffer.StrideInBytes = vertexBuffer->GetVertexStride();
			geometryDesc[i].Triangles.VertexCount = static_cast<UINT>(vertexBuffer->GetNumVertices());
			geometryDesc[i].Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			geometryDesc[i].Triangles.IndexBuffer = indexBuffer->GetD3D12Resource()->GetGPUVirtualAddress();
			geometryDesc[i].Triangles.IndexFormat = indexBuffer->GetIndexBufferView().Format;
			geometryDesc[i].Triangles.IndexCount = static_cast<UINT>(indexBuffer->GetNumIndices());
			geometryDesc[i].Triangles.Transform3x4 = 0;
			geometryDesc[i].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		}
		else {
			spdlog::warn("Ray tracing geometry descriptor size reached: {}", geometryDescSize);
		}
	}

	//Get size requirements for scratch and Acceleration Structure Buffers
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = geometryDescSize;
	inputs.pGeometryDescs = geometryDesc.data();
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
	rtxDevice->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	wrl::ComPtr<ID3D12Resource> pScratch;
	ThrowIfFailed(rtxDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&pScratch)));

	CEResourceStateTracker::AddGlobalResourceState(pScratch.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	wrl::ComPtr<ID3D12Resource> pResult;
	ThrowIfFailed(rtxDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nullptr,
		IID_PPV_ARGS(&pResult)));

	CEResourceStateTracker::AddGlobalResourceState(pResult.Get(),
	                                               D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

	//Create bottom level AS
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
	asDesc.Inputs = inputs;
	asDesc.DestAccelerationStructureData = pResult->GetGPUVirtualAddress();
	asDesc.ScratchAccelerationStructureData = pScratch->GetGPUVirtualAddress();

	rtxCommandList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

	// //Wait for BLAS build to complete
	if (GetCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT) {
		commandList->UAVBarrier(pResult, true);
	}
	else {
		UAVBarrier(pResult, true);
	}

	AccelerationStructureBuffers ACBuffer = {pScratch, pResult};
	return ACBuffer;
}

void CECommandList::CreateTopLevelAccelerationStructure(float rotation,
                                                        bool update,
                                                        AccelerationStructureBuffers& buffer,
                                                        AccelerationStructureBuffers bottomLvlBuffers,
                                                        bool flush) {
	auto rtxDevice = m_device.GetDevice();
	wrl::ComPtr<ID3D12GraphicsCommandList5> rtxCommandList;
	std::shared_ptr<CECommandList> commandList;
	if (GetCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT) {
		auto& commandQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		commandList = commandQueue.GetCommandList();
		rtxCommandList = commandList->GetCommandList();
	}
	else {
		rtxCommandList = commandList->GetCommandList();
	}

	//Get size of Top Level Acceleration Structure and create them
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = CESwapChain::BufferCount;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
	rtxDevice->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	if (update) {
		//If structure is a request for update, then TLAS was already used in DispatchRay() call. We need a UAV barrier to make sure the read operation ends before updating the buffer
		if (GetCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT) {
			commandList->UAVBarrier(buffer.pResult, true);
		}
		else {
			UAVBarrier(buffer.pResult, true);
		}
	}
	else {
		//If structure is not a request for update, then we need to create the buffers, otherwise we will refit in-place
		wrl::ComPtr<ID3D12Resource> pScratch;
		ThrowIfFailed(rtxDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&pScratch)));

		CEResourceStateTracker::AddGlobalResourceState(pScratch.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		wrl::ComPtr<ID3D12Resource> pResult;
		ThrowIfFailed(rtxDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nullptr,
			IID_PPV_ARGS(&pResult)));

		wrl::ComPtr<ID3D12Resource> pInstanceDesc;
		ThrowIfFailed(rtxDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * 3,
			                               D3D12_RESOURCE_FLAG_NONE),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pInstanceDesc)));

		CEResourceStateTracker::AddGlobalResourceState(pResult.Get(),
		                                               D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

		buffer.pScratch = pScratch;
		buffer.pResult = pResult;
		buffer.pInstanceDesc = pInstanceDesc;
		buffer.mTlasSize = info.ResultDataMaxSizeInBytes;
	}

	//Map instance desc buffer
	D3D12_RAYTRACING_INSTANCE_DESC* instanceDescs;
	buffer.pInstanceDesc->Map(0, nullptr, (void**)&instanceDescs);
	ZeroMemory(instanceDescs, sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * 3);

	//transform matrices for instances
	XMMATRIX transformation[3];
	XMMATRIX rotationMat = XMMatrixRotationY(rotation);
	transformation[0] = XMMatrixIdentity();
	transformation[1] = XMMatrixTranslation(-2, 0, 0) * rotationMat;
	transformation[2] = XMMatrixTranslation(2, 0, 0) * rotationMat;

	//Instance Contribution To Hit Group Index is set based on shader table layout specified in create createShaderTable()
	//Create descriptor for objects instance
	for (uint32_t i = 1; i < 3; i++) {
		instanceDescs[i].InstanceID = i; // This value will be exposed to the shader via InstanceID()
		instanceDescs[i].InstanceContributionToHitGroupIndex = (i * 2) + 2;
		// The indices are relative to to the start of the hit-table entries specified in Raytrace(), so we need 4 and 6
		instanceDescs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		XMMATRIX m = XMMatrixTranspose(transformation[i]);
		memcpy(instanceDescs[i].Transform, &m, sizeof(instanceDescs[i].Transform));
		instanceDescs[i].AccelerationStructure = bottomLvlBuffers.pResult->GetGPUVirtualAddress();
		instanceDescs[i].InstanceMask = 0xFF;
	}

	buffer.pInstanceDesc->Unmap(0, nullptr);

	//Create TLAS
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
	asDesc.Inputs = inputs;
	asDesc.Inputs.InstanceDescs = buffer.pInstanceDesc->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = buffer.pResult->GetGPUVirtualAddress();
	asDesc.ScratchAccelerationStructureData = buffer.pScratch->GetGPUVirtualAddress();

	//If update operation, set source buffer and perform update flag
	if (update) {
		asDesc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		asDesc.SourceAccelerationStructureData = buffer.pResult->GetGPUVirtualAddress();
	}

	rtxCommandList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

	if (GetCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT) {
		commandList->UAVBarrier(buffer.pResult, true);
	}
	else {
		UAVBarrier(buffer.pResult, true);
	}

	// if (flush) {
	// 	commandQueue.ExecuteCommandList(commandList);
	// 	commandQueue.Flush();
	// }
}

void CECommandList::ClearTexture(const std::shared_ptr<CETexture>& texture, const float clearColor[4]) {
	assert(texture);

	TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
	m_commandList->ClearRenderTargetView(texture->GetRenderTargetView(), clearColor, 0, nullptr);

	TrackResource(texture);
}

void CECommandList::ClearDepthStencilTexture(const std::shared_ptr<CETexture>& texture, D3D12_CLEAR_FLAGS clearFlags,
                                             float depth, uint8_t stencil) {
	assert(texture);

	TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
	m_commandList->ClearDepthStencilView(texture->GetDepthStencilView(), clearFlags, depth, stencil, 0, nullptr);

	TrackResource(texture);
}

void CECommandList::CopyTextureSubResource(const std::shared_ptr<CETexture>& texture, uint32_t firstSubresource,
                                           uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData) {
	assert(texture);

	auto d3d12Device = m_device.GetDevice();
	auto destinationResource = texture->GetD3D12Resource();

	if (destinationResource) {
		// Resource must be in the copy-destination state.
		TransitionBarrier(texture, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		UINT64 requiredSize =
			GetRequiredIntermediateSize(destinationResource.Get(), firstSubresource, numSubresources);

		// Create a temporary (intermediate) resource for uploading the subresources
		ComPtr<ID3D12Resource> intermediateResource;
		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&intermediateResource)));

		UpdateSubresources(m_commandList.Get(), destinationResource.Get(), intermediateResource.Get(), 0,
		                   firstSubresource, numSubresources, subresourceData);

		TrackResource(intermediateResource);
		TrackResource(destinationResource);
	}
}

void CECommandList::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes,
                                                     const void* bufferData) {
	// Constant buffers must be 256-byte aligned.
	auto heapAllococation = m_uploadBuffer->Allocate(sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(heapAllococation.CPU, bufferData, sizeInBytes);

	m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, heapAllococation.GPU);
}

void CECommandList::SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants,
                                              const void* constants) {
	m_commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, numConstants, constants, 0);
}

void CECommandList::SetCompute32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants,
                                             const void* constants) {
	m_commandList->SetComputeRoot32BitConstants(rootParameterIndex, numConstants, constants, 0);
}

void CECommandList::SetVertexBuffers(uint32_t startSlot,
                                     const std::vector<std::shared_ptr<CEVertexBuffer>>& vertexBuffers) {
	std::vector<D3D12_VERTEX_BUFFER_VIEW> views;
	views.reserve(vertexBuffers.size());

	for (auto vertexBuffer : vertexBuffers) {
		if (vertexBuffer) {
			TransitionBarrier(vertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			TrackResource(vertexBuffer);

			views.push_back(vertexBuffer->GetVertexBufferView());
		}
	}

	m_commandList->IASetVertexBuffers(startSlot, views.size(), views.data());
}

void CECommandList::SetVertexBuffer(uint32_t slot, const std::shared_ptr<CEVertexBuffer>& vertexBuffer) {
	SetVertexBuffers(slot, {vertexBuffer});
}

void CECommandList::SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize,
                                           const void* vertexBufferData) {
	size_t bufferSize = numVertices * vertexSize;

	auto heapAllocation = m_uploadBuffer->Allocate(bufferSize, vertexSize);
	memcpy(heapAllocation.CPU, vertexBufferData, bufferSize);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	vertexBufferView.BufferLocation = heapAllocation.GPU;
	vertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
	vertexBufferView.StrideInBytes = static_cast<UINT>(vertexSize);

	m_commandList->IASetVertexBuffers(slot, 1, &vertexBufferView);
}

void CECommandList::SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer) {
	if (indexBuffer) {
		TransitionBarrier(indexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		TrackResource(indexBuffer);
		m_commandList->IASetIndexBuffer(&(indexBuffer->GetIndexBufferView()));
	}
}

void CECommandList::SetDynamicIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData) {
	size_t indexSizeInBytes = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
	size_t bufferSize = numIndicies * indexSizeInBytes;

	auto heapAllocation = m_uploadBuffer->Allocate(bufferSize, indexSizeInBytes);
	memcpy(heapAllocation.CPU, indexBufferData, bufferSize);

	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	indexBufferView.BufferLocation = heapAllocation.GPU;
	indexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
	indexBufferView.Format = indexFormat;

	m_commandList->IASetIndexBuffer(&indexBufferView);
}

void CECommandList::SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t numElements, size_t elementSize,
                                                       const void* bufferData) {
	size_t bufferSize = numElements * elementSize;

	auto heapAllocation = m_uploadBuffer->Allocate(bufferSize, elementSize);

	memcpy(heapAllocation.CPU, bufferData, bufferSize);

	m_commandList->SetGraphicsRootShaderResourceView(slot, heapAllocation.GPU);
}

void CECommandList::SetViewport(const D3D12_VIEWPORT& viewport) {
	SetViewports({viewport});
}

void CECommandList::SetViewports(const std::vector<D3D12_VIEWPORT>& viewports) {
	assert(viewports.size() < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	m_commandList->RSSetViewports(static_cast<UINT>(viewports.size()), viewports.data());
}

void CECommandList::SetScissorRect(const D3D12_RECT& scissorRect) {
	SetScissorRects({scissorRect});
}

void CECommandList::SetScissorRects(const std::vector<D3D12_RECT>& scissorRects) {
	assert(scissorRects.size() < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	m_commandList->RSSetScissorRects(static_cast<UINT>(scissorRects.size()), scissorRects.data());
}

void CECommandList::SetPipelineState(const std::shared_ptr<CEPipelineStateObject>& pipelineState) {
	assert(pipelineState);

	auto d3d12PipelineStateObject = pipelineState->GetD3D12PipelineState().Get();
	if (m_pipelineState != d3d12PipelineStateObject) {
		m_pipelineState = d3d12PipelineStateObject;

		m_commandList->SetPipelineState(d3d12PipelineStateObject);

		TrackResource(d3d12PipelineStateObject);
	}
}

void CECommandList::SetGraphicsRootSignature(const std::shared_ptr<CERootSignature>& rootSignature) {
	assert(rootSignature);

	auto d3d12RootSignature = rootSignature->GetD3D12RootSignature().Get();
	if (m_rootSignature != d3d12RootSignature) {
		m_rootSignature = d3d12RootSignature;

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
			m_dynamicDescriptorHeap[i]->ParseRootSignature(rootSignature);
		}

		m_commandList->SetGraphicsRootSignature(m_rootSignature);

		TrackResource(m_rootSignature);
	}
}

void CECommandList::SetComputeRootSignature(const std::shared_ptr<CERootSignature>& rootSignature) {
	assert(rootSignature);

	auto d3d12RootSignature = rootSignature->GetD3D12RootSignature().Get();
	if (m_rootSignature != d3d12RootSignature) {
		m_rootSignature = d3d12RootSignature;

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
			m_dynamicDescriptorHeap[i]->ParseRootSignature(rootSignature);
		}

		m_commandList->SetComputeRootSignature(m_rootSignature);

		TrackResource(m_rootSignature);
	}
}

void CECommandList::SetConstantBufferView(uint32_t rootParameterIndex, const std::shared_ptr<CEConstantBuffer>& buffer,
                                          D3D12_RESOURCE_STATES stateAfter, size_t bufferOffset) {
	if (buffer) {
		auto d3d12Resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12Resource, stateAfter);

		m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineCBV(
			rootParameterIndex, d3d12Resource->GetGPUVirtualAddress() + bufferOffset);

		TrackResource(buffer);
	}
}

void CECommandList::SetShaderResourceView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
                                          D3D12_RESOURCE_STATES stateAfter, size_t bufferOffset) {
	if (buffer) {
		auto d3d12Resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12Resource, stateAfter);

		m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineSRV(
			rootParameterIndex, d3d12Resource->GetGPUVirtualAddress() + bufferOffset);

		TrackResource(buffer);
	}
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
                                           D3D12_RESOURCE_STATES stateAfter, size_t bufferOffset) {
	if (buffer) {
		auto d3d12Resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12Resource, stateAfter);

		m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineUAV(
			rootParameterIndex, d3d12Resource->GetGPUVirtualAddress() + bufferOffset);

		TrackResource(buffer);
	}
}

void CECommandList::SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CEShaderResourceView>& srv,
                                          D3D12_RESOURCE_STATES stateAfter, UINT firstSubresource,
                                          UINT numSubresources) {
	assert(srv);

	auto resource = srv->GetResource();
	if (resource) {
		if (numSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < numSubresources; ++i) {
				TransitionBarrier(resource, stateAfter, firstSubresource + i);
			}
		}
		else {
			TransitionBarrier(resource, stateAfter);
		}

		TrackResource(resource);
	}

	m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
		rootParameterIndex, descriptorOffset, 1, srv->GetDescriptorHandle());
}

void CECommandList::SetShaderResourceView(int32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CETexture>& texture, D3D12_RESOURCE_STATES stateAfter,
                                          UINT firstSubresource, UINT numSubresources) {

	if (texture) {
		if (numSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < numSubresources; ++i) {
				TransitionBarrier(texture, stateAfter, firstSubresource + i);
			}
		}
		else {
			TransitionBarrier(texture, stateAfter);
		}

		TrackResource(texture);

		m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			rootParameterIndex, descriptorOffset, 1, texture->GetShaderResourceView());
	}
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                           const std::shared_ptr<CEUnorderedAccessView>& uav,
                                           D3D12_RESOURCE_STATES stateAfter, UINT firstSubresource,
                                           UINT numSubresources) {
	assert(uav);

	auto resource = uav->GetResource();
	if (resource) {
		if (numSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < numSubresources; ++i) {
				TransitionBarrier(resource, stateAfter, firstSubresource + i);
			}
		}
		else {
			TransitionBarrier(resource, stateAfter);
		}

		TrackResource(resource);
	}

	m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
		rootParameterIndex, descriptorOffset, 1, uav->GetDescriptorHandle());
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                           const std::shared_ptr<CETexture>& texture, UINT mip,
                                           D3D12_RESOURCE_STATES stateAfter, UINT firstSubresource,
                                           UINT numSubresources) {
	if (texture) {
		if (numSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < numSubresources; ++i) {
				TransitionBarrier(texture, stateAfter, firstSubresource + i);
			}
		}
		else {
			TransitionBarrier(texture, stateAfter);
		}

		TrackResource(texture);

		m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			rootParameterIndex, descriptorOffset, 1, texture->GetUnorderedAccessView(mip));
	}
}

void CECommandList::SetConstantBufferView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CEConstantBufferView>& cbv,
                                          D3D12_RESOURCE_STATES stateAfter) {
	assert(cbv);

	auto constantBuffer = cbv->GetConstantBuffer();
	if (constantBuffer) {
		TransitionBarrier(constantBuffer, stateAfter);
		TrackResource(constantBuffer);
	}

	m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
		rootParameterIndex, descriptorOffset, 1, cbv->GetDescriptorHandle());
}

void CECommandList::SetRenderTarget(const CERenderTarget& renderTarget) {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors;
	renderTargetDescriptors.reserve(AttachmentPoint::NumAttachmentPoints);

	const auto& textures = renderTarget.GetTextures();

	// Bind color targets (max of 8 render targets can be bound to the rendering pipeline.
	for (int i = 0; i < 8; ++i) {
		auto texture = textures[i];

		if (texture) {
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
			renderTargetDescriptors.push_back(texture->GetRenderTargetView());

			TrackResource(texture);
		}
	}

	auto depthTexture = renderTarget.GetTexture(AttachmentPoint::DepthStencil);

	CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor(D3D12_DEFAULT);
	if (depthTexture) {
		TransitionBarrier(depthTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		depthStencilDescriptor = depthTexture->GetDepthStencilView();

		TrackResource(depthTexture);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE* pDSV = depthStencilDescriptor.ptr != 0 ? &depthStencilDescriptor : nullptr;

	m_commandList->OMSetRenderTargets(static_cast<UINT>(renderTargetDescriptors.size()),
	                                  renderTargetDescriptors.data(), FALSE, pDSV);
}

void CECommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i]->CommitStagedDescriptorsForDraw(*this);
	}

	m_commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
}

void CECommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex,
                                uint32_t startInstance) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i]->CommitStagedDescriptorsForDraw(*this);
	}

	m_commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
}

void CECommandList::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i]->CommitStagedDescriptorsForDispatch(*this);
	}

	m_commandList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

bool CECommandList::Close(const std::shared_ptr<CECommandList>& pendingCommandList) {
	// Flush any remaining barriers.
	FlushResourceBarriers();

	m_commandList->Close();

	// Flush pending resource barriers.
	uint32_t numPendingBarriers = m_resourceStateTracker->FlushPendingResourceBarriers(pendingCommandList);
	// Commit the final resource state to the global state.
	m_resourceStateTracker->CommitFinalResourceStates();

	return numPendingBarriers > 0;
}

void CECommandList::Close() {
	FlushResourceBarriers();
	m_commandList->Close();
}

void CECommandList::Reset() {
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	m_resourceStateTracker->Reset();
	m_uploadBuffer->Reset();

	ReleaseTrackedObjects();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i]->Reset();
		m_descriptorHeaps[i] = nullptr;
	}

	m_rootSignature = nullptr;
	m_pipelineState = nullptr;
	m_computeCommandList = nullptr;
}

void CECommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object) {
	m_trackedObjects.push_back(object);
}

void CECommandList::TrackResource(const std::shared_ptr<CEResource>& res) {
	assert(res);

	TrackResource(res->GetD3D12Resource());
}

void CECommandList::ReleaseTrackedObjects() {
	m_trackedObjects.clear();
}

void CECommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap) {
	if (m_descriptorHeaps[heapType] != heap) {
		m_descriptorHeaps[heapType] = heap;
		BindDescriptorHeaps();
	}
}

void CECommandList::BindDescriptorHeaps() {
	UINT numDescriptorHeaps = 0;
	ID3D12DescriptorHeap* descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		ID3D12DescriptorHeap* descriptorHeap = m_descriptorHeaps[i];
		if (descriptorHeap) {
			descriptorHeaps[numDescriptorHeaps++] = descriptorHeap;
		}
	}

	m_commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps);
}
