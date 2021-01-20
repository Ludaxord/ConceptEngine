#include "CECommandList.h"

#include "CEGenerateMipsPSO.h"
#include "CECommandQueue.h"
#include "CEDX12Libs.h"
#include "CEDevice.h"
#include "CEDynamicDescriptorHeap.h"
#include "CEHelper.h"
#include "CEPanoToCubemapPSO.h"
#include "CEResource.h"
#include "CEResourceStateTracker.h"
#include "CEScene.h"
#include "CETexture.h"
#include "CEUploadBuffer.h"


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

void CECommandList::TransitionBarrier(const std::shared_ptr<CEResource>& resource,
                                      D3D12_RESOURCE_STATES stateAfter, UINT subResource, bool flushBarriers) {
	if (resource) {
		TransitionBarrier(resource->GetD3D12Resource(), stateAfter, subResource, flushBarriers);
	}
}

void CECommandList::TransitionBarrier(wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter,
                                      UINT subResource, bool flushBarriers) {
	if (resource) {
		/*
		 * before state will be resolved by resource state tracker;
		 */
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, stateAfter,
		                                                    subResource);
		m_resourceStateTracker->ResourceBarrier(barrier);
	}

	if (flushBarriers) {
		FlushResourceBarriers();
	}
}

void CECommandList::UAVBarrier(const std::shared_ptr<CEResource>& resource, bool flushBarriers) {
	auto d3d12Resource = resource ? resource->GetD3D12Resource() : nullptr;
	UAVBarrier(d3d12Resource, flushBarriers);
}

void CECommandList::UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource, bool flushBarriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());
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

void CECommandList::AliasingBarrier(wrl::ComPtr<ID3D12Resource> beforeResource,
                                    wrl::ComPtr<ID3D12Resource>& afterResource, bool flushBarriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(beforeResource.Get(), afterResource.Get());
	m_resourceStateTracker->ResourceBarrier(barrier);
	if (flushBarriers) {
		FlushResourceBarriers();
	}
}

void CECommandList::FlushResourceBarriers() {
	m_resourceStateTracker->FlushResourceBarriers(shared_from_this());
}

void CECommandList::CopyResource(const std::shared_ptr<CEResource>& destinationResource,
                                 const std::shared_ptr<CEResource>& sourceResource) {
	assert(destinationResource && sourceResource);
	CopyResource(destinationResource->GetD3D12Resource(), sourceResource->GetD3D12Resource());
}

void CECommandList::CopyResource(wrl::ComPtr<ID3D12Resource> destinationResource,
                                 wrl::ComPtr<ID3D12Resource> sourceResource) {
	assert(destinationResource);
	assert(sourceResource);

	TransitionBarrier(destinationResource, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(sourceResource, D3D12_RESOURCE_STATE_COPY_SOURCE);

	FlushResourceBarriers();

	m_commandList->CopyResource(destinationResource.Get(), sourceResource.Get());

	TrackResource(destinationResource);
	TrackResource(sourceResource);
}

void CECommandList::ResolveSubResource(const std::shared_ptr<CEResource>& destinationResource,
                                       const std::shared_ptr<CEResource>& sourceResource,
                                       uint32_t destinationSubResource,
                                       uint32_t sourceSubResource) {
	assert(destinationResource && sourceResource);

	TransitionBarrier(destinationResource, D3D12_RESOURCE_STATE_RESOLVE_DEST, destinationSubResource);
	TransitionBarrier(sourceResource, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, sourceSubResource);

	FlushResourceBarriers();

	m_commandList->ResolveSubresource(destinationResource->GetD3D12Resource().Get(),
	                                  destinationSubResource,
	                                  sourceResource->GetD3D12Resource().Get(),
	                                  sourceSubResource,
	                                  destinationResource->GetD3D12ResourceDesc().Format
	);

	TrackResource(sourceResource);
	TrackResource(destinationResource);
}

std::shared_ptr<CEVertexBuffer> CECommandList::CopyVertexBuffer(size_t numVertices, size_t vertexStride,
                                                                const void* vertexBufferData) {
	auto d3d12Resource = CopyBuffer(numVertices * vertexStride, vertexBufferData);
	std::shared_ptr<CEVertexBuffer> vertexBuffer = m_device.
		CreateVertexBuffer(d3d12Resource, numVertices, vertexStride);
	return vertexBuffer;
}

std::shared_ptr<CEIndexBuffer> CECommandList::CopyIndexBuffer(size_t numIndices, DXGI_FORMAT indexFormat,
                                                              const void* indexBufferData) {
	size_t elementSize = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
	auto d3d12Resource = CopyBuffer(numIndices * elementSize, indexBufferData);
	std::shared_ptr<CEIndexBuffer> indexBuffer = m_device.CreateIndexBuffer(d3d12Resource, numIndices, indexFormat);
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
	auto d3d12Resource = CopyBuffer(numElements * elementSize, bufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	std::shared_ptr<CEStructuredBuffer> structuredBuffer = m_device.CreateStructuredBuffer(
		d3d12Resource, numElements, elementSize);
}

void CECommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology) {
	m_commandList->IASetPrimitiveTopology(primitiveTopology);
}

std::shared_ptr<CETexture> CECommandList::LoadTextureFromFile(const std::wstring& fileName, bool sRGB) {
	std::shared_ptr<CETexture> texture;
	fs::path filePath(fileName);
	if (!fs::exists(filePath)) {
		throw std::exception("File not found");
	}

	std::lock_guard<std::mutex> lock(m_textureCacheMutex);
	auto iter = m_textureCache.find(fileName);
	if (iter != m_textureCache.end()) {
		texture = m_device.CreateTexture(iter->second);
	}
	else {
		TexMetadata metadata;
		ScratchImage scratchImage;
		const auto extension = filePath.extension();
		if (extension == ".dds") {
			ThrowIfFailed(LoadFromDDSFile(fileName.c_str(), DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}
		else if (extension == ".hdr") {
			ThrowIfFailed(LoadFromHDRFile(fileName.c_str(), &metadata, scratchImage));
		}
		else if (extension == ".tga") {
			ThrowIfFailed(LoadFromTGAFile(fileName.c_str(), &metadata, scratchImage));
		}
		else {
			ThrowIfFailed(LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}

		/*
		 * Force texture format to be sRGB to convert to linear when sampling texture in a shader.
		 */
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
		wrl::ComPtr<ID3D12Resource> textureResource;

		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureResource)
		));

		texture = m_device.CreateTexture(textureResource);
		texture->SetName(fileName);

		/*
		 * Update global state tracker
		 */
		CEResourceStateTracker::AddGlobalResourceState(textureResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		std::vector<D3D12_SUBRESOURCE_DATA> subResources(scratchImage.GetImageCount());
		const Image* pImages = scratchImage.GetImages();
		for (int i = 0; i < scratchImage.GetImageCount(); ++i) {
			auto& subResource = subResources[i];
			subResource.RowPitch = pImages[i].rowPitch;
			subResource.SlicePitch = pImages[i].slicePitch;
			subResource.pData = pImages[i].pixels;
		}

		CopyTextureSubResource(texture, 0, static_cast<uint32_t>(subResources.size()), subResources.data());
		if (subResources.size() < textureResource->GetDesc().MipLevels) {
			GenerateMips(texture);
		}

		/*
		 * Add texture resource to texture cache.
		 */
		m_textureCache[fileName] = textureResource.Get();
	}

	return texture;
}

std::shared_ptr<CEScene> CECommandList::LoadSceneFromFile(const std::wstring& fileName,
                                                          const std::function<bool(float)>& loadingProgress) {
	auto scene = std::make_shared<CEScene>();
	if (scene->LoadSceneFromFile(*this, fileName, loadingProgress)) {
		return scene;
	}
	return nullptr;
}

std::shared_ptr<CEScene> CECommandList::LoadSceneFromString(const std::string& sceneString,
                                                            const std::string& format) {
	auto scene = std::make_shared<CEScene>();
	scene->LoadSceneFromString(*this, sceneString, format);
	return scene;
}

std::shared_ptr<CEScene> CECommandList::CreateCube(float size, bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateSphere(float radius, uint32_t tesselation, bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateCylinder(float radius, float height, uint32_t tessellation,
                                                       bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateCone(float radius, float height, uint32_t tessellation,
                                                   bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateQuad(float size, bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateTriangle(float size, bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateTorus(float radius, float thickness, uint32_t tessellation,
                                                    bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreatePlane(float width, float height, bool reverseWinding) {
}

std::shared_ptr<CEScene> CECommandList::CreateCar(float width, float height, bool reverseWinding) {
}

void CECommandList::ClearTexture(const std::shared_ptr<CETexture>& texture, const float clearColor[4]) {
}

void CECommandList::ClearDepthStencilTexture(const std::shared_ptr<CETexture>& texture,
                                             D3D12_CLEAR_FLAGS clearFlags, float depth, uint8_t stencil) {
}

void CECommandList::GenerateMips(const std::shared_ptr<CETexture> texture) {
	if (!texture) {
		return;
	}

	auto d3d12Device = m_device.GetDevice();
	if (m_commandListType == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_computeCommandList) {
			m_computeCommandList = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}
		m_computeCommandList->GenerateMips(texture);
		return;
	}

	auto d3d12Resource = texture->GetD3D12Resource();
	/*
	 * if texture does not have valid resource, then quit
	 */
	if (!d3d12Resource) {
		return;
	}
	auto resourceDesc = d3d12Resource->GetDesc();
	/*
	 * If texture only has single mip level (level 0), then quit
	 */
	if (resourceDesc.MipLevels == 1) {
		return;
	}
	/*
	 * Only non-multi-sampled 2D textures are supported for now.
	 */
	if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || resourceDesc.DepthOrArraySize != 1 ||
		resourceDesc.SampleDesc.Count > 1) {
		throw std::exception("GenerateMips is only supported fo non-multi-sampled 2D textures.");
	}

	wrl::ComPtr<ID3D12Resource> uavResource = d3d12Resource;
	/*
	 * Create alias of original resource, to perform GPU copy of resources with different formats.
	 * BGR -> RGB texture copies will fail GPU validation
	 * unless performed through an alias of BRG resource in placed heap
	 */
	wrl::ComPtr<ID3D12Resource> aliasResource;

	/*
	 * if passed-in resource does not allow for UAV access
	 * then create staging resource that is used to generate
	 * mipmap chain
	 */
	if (!texture->CheckUAVSupport() || (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		/*
		 * Describe alias resource that is used to copy original texture.
		 */
		auto aliasDesc = resourceDesc;
		/*
		 * Placed resource can not be render target or depth-stencil views.
		 */
		aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		/*
		 * Describe UAV compatible resource that is used to perform
		 * mipMapping of original texture.
		 */
		/*
		 * flags for UAV description must match that of alias description.
		 */
		auto uavDesc = aliasDesc;
		uavDesc.Format = CETexture::GetUAVComatibleFormat(resourceDesc.Format);

		D3D12_RESOURCE_DESC resourceDescs[] = {aliasDesc, uavDesc};

		/*
		 * Create heap that is large enough to store a copy of original resource.
		 */
		auto allocationInfo = d3d12Device->GetResourceAllocationInfo(0, _countof(resourceDescs), resourceDescs);

		D3D12_HEAP_DESC heapDesc = {};
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Alignment = allocationInfo.Alignment;
		heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
		// heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

		wrl::ComPtr<ID3D12Heap> heap;
		ThrowIfFailed(d3d12Device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));

		/*
		 * Make sure that heap does not go out of scope until command list, finished executing on command queue.
		 */
		TrackResource(heap);

		/*
		 * Create a placed resource that matches description of original resource.
		 * This resource is used to copy original texture to UAV compatible resource.
		 */
		ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(), 0, &aliasDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
		                                                IID_PPV_ARGS(&aliasResource)));

		CEResourceStateTracker::AddGlobalResourceState(aliasResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		/*
		 * Ensure scope of alias resource.
		 */
		TrackResource(aliasResource);

		/*
		 * Create UAV compatible resource in same heap as alias resource.
		 */
		ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(),
		                                                0,
		                                                &uavDesc,
		                                                D3D12_RESOURCE_STATE_COMMON,
		                                                nullptr,
		                                                IID_PPV_ARGS(&uavResource)));
		CEResourceStateTracker::AddGlobalResourceState(uavResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		/*
		 * Ensure scope of UAV compatible resource.
		 */
		TrackResource(uavResource);

		/*
		 * Add aliasing barrier for alias resource.
		 */
		AliasingBarrier(nullptr, aliasResource);

		/*
		 * Copy original resource to alias resource.
		 * this ensures GPU validation.
		 */
		CopyResource(aliasResource, d3d12Resource);

		/*
		 * Add aliasing barrier for UAV compatible resource.
		 */
		AliasingBarrier(aliasResource, uavResource);
	}

	/*
	 * generate mips with UAV compatible resource.
	 */
	auto uavTexture = m_device.CreateTexture(uavResource);
	GenerateMipsUAV(uavTexture, CETexture::IsSRGBFormat(resourceDesc.Format));

	if (aliasResource) {
		AliasingBarrier(uavResource, aliasResource);
		/*
		 * Copy alias resource back to original resource
		 */
		CopyResource(d3d12Resource, aliasResource);
	}
}

void CECommandList::PanoToCubeMap(const std::shared_ptr<CETexture>& cubeMapTexture,
                                  const std::shared_ptr<CETexture>& panoTexture) {
	assert(cubeMapTexture && panoTexture);
	if (m_commandListType == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_computeCommandList) {
			m_computeCommandList = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}

		m_computeCommandList->PanoToCubeMap(cubeMapTexture, panoTexture);
		return;
	}

	if (!m_panoToCubemapPSO) {
		m_panoToCubemapPSO = std::make_unique<CEPanoToCubemapPSO>(m_device);
	}

	auto cubeMapResource = cubeMapTexture->GetD3D12Resource();
	if (!cubeMapResource) {
		return;
	}

	CD3DX12_RESOURCE_DESC cubeMapDesc(cubeMapResource->GetDesc());
	auto stagingResource = cubeMapResource;
	auto stagingTexture = m_device.CreateTexture(stagingResource);

	/*
	 * If passed resource does not allow for UAV access
	 * then create staging resource that is used to generate cubemap
	 */
	if ((cubeMapDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		auto d3d12Device = m_device.GetDevice();
		auto stagingDesc = cubeMapDesc;
		stagingDesc.Format = CETexture::GetUAVComatibleFormat(cubeMapDesc.Format);
		stagingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		ThrowIfFailed(d3d12Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		                                                   D3D12_HEAP_FLAG_NONE, &stagingDesc,
		                                                   D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
		                                                   IID_PPV_ARGS(&stagingResource)));
		CEResourceStateTracker::AddGlobalResourceState(stagingResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);

		stagingTexture = m_device.CreateTexture(stagingResource);
		stagingTexture->SetName(L"Pano to CubeMap Staging Texture");

		CopyResource(stagingTexture, cubeMapTexture);
	}

	TransitionBarrier(stagingResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	SetPipelineState(m_panoToCubemapPSO->GetPipelineState());
	SetComputeRootSignature(m_panoToCubemapPSO->GetRootSignature());

	PanoToCubemapCB panoToCubemapCB;
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = CETexture::GetUAVComatibleFormat(cubeMapDesc.Format);
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.ArraySize = 6;

	auto srv = m_device.CreateShaderResourceView(panoTexture);
	SetShaderResourceView(PanoToCubemapRS::SrcTexture, 0, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	for (uint32_t mipSlice = 0; mipSlice < cubeMapDesc.MipLevels;) {
		/*
		 * Maximum number of mips to generate per pas is 5.
		 */
		uint32_t numMips = std::min<uint32_t>(5, cubeMapDesc.MipLevels - mipSlice);

		panoToCubemapCB.FirstMip = mipSlice;
		panoToCubemapCB.CubemapSize = std::max<uint32_t>(static_cast<uint32_t>(cubeMapDesc.Width), cubeMapDesc.Height)
			>> mipSlice;
		SetCompute32BitConstants(PanoToCubemapRS::PanoToCubemapCB, panoToCubemapCB);
		for (uint32_t mip = 0; mip < numMips; ++mip) {
			uavDesc.Texture2DArray.MipSlice = mipSlice + mip;
			auto uav = m_device.CreateUnorderedAccessView(stagingTexture, nullptr, &uavDesc);
			SetUnorderedAccessView(PanoToCubemapRS::DstMips, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 0, 0);
		}

		if (numMips < 5) {
			/*
			 * Pad unused mips
			 */
			m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
				PanoToCubemapRS::DstMips, panoToCubemapCB.NumMips, 5 - numMips, m_panoToCubemapPSO->GetDefaultUAV());
		}

		Dispatch(Math::DivideByMultiple(panoToCubemapCB.CubemapSize, 16),
		         Math::DivideByMultiple(panoToCubemapCB.CubemapSize, 16), 6);
		mipSlice += numMips;
	}

	if (stagingResource != cubeMapResource) {
		CopyResource(cubeMapTexture, stagingTexture);
	}
}

void CECommandList::CopyTextureSubResource(const std::shared_ptr<CETexture>& texture,
                                           uint32_t firstSubResource, uint32_t numSubResources,
                                           D3D12_SUBRESOURCE_DATA* subResourceData) {
}

void CECommandList::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes,
                                                     const void* bufferData) {
}

void CECommandList::SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants,
                                              const void* constants) {
}

void CECommandList::SetCompute32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants,
                                             const void* constants) {
}

void CECommandList::SetVertexBuffers(uint32_t startSlot,
                                     const std::vector<std::shared_ptr<CEVertexBuffer>>& vertexBufferViews) {
}

void CECommandList::SetVertexBuffer(uint32_t slot, const std::shared_ptr<CEVertexBuffer>& vertexBufferView) {
}

void CECommandList::SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize,
                                           const void* vertexBufferData) {
}

void CECommandList::SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer) {
}

void CECommandList::SetDynamicIndexBuffer(size_t numIndicates, DXGI_FORMAT indexFormat,
                                          const void* indexBufferData) {
}

void CECommandList::SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t numElements, size_t elementSize,
                                                       const void* bufferData) {
}

void CECommandList::SetViewport(const D3D12_VIEWPORT& viewport) {
}

void CECommandList::SetViewports(const std::vector<D3D12_VIEWPORT>& viewports) {
}

void CECommandList::SetScissorRect(const D3D12_RECT& scissorRect) {
}

void CECommandList::SetScissorRects(const std::vector<D3D12_RECT>& scissorRects) {
}

void CECommandList::SetPipelineState(const std::shared_ptr<CEPipelineStateObject>& pipelineState) {
}

void CECommandList::SetGraphicsRootSignature(const std::shared_ptr<CERootSignature>& rootSignature) {
}

void CECommandList::SetComputeRootSignature(const std::shared_ptr<CERootSignature>& rootSignature) {
}

void CECommandList::SetConstantBufferView(uint32_t rootParameterIndex,
                                          const std::shared_ptr<CEConstantBuffer>& buffer,
                                          D3D12_RESOURCE_STATES stateAfter, uint32_t bufferOffset) {
}

void CECommandList::SetShaderResourceView(uint32_t rootParameterIndex,
                                          const std::shared_ptr<CEBuffer>& buffer,
                                          D3D12_RESOURCE_STATES stateAfter, uint32_t bufferOffset) {
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex,
                                           const std::shared_ptr<CEBuffer>& buffer,
                                           D3D12_RESOURCE_STATES stateAfter, size_t bufferOffset) {
}

void CECommandList::SetConstantBufferView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CEConstantBufferView>& cbv,
                                          D3D12_RESOURCE_STATES stateAfter) {
}

void CECommandList::SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CEShaderResourceView>& srv,
                                          D3D12_RESOURCE_STATES stateAfter, UINT firstSubResource,
                                          UINT numSubResources) {
}

void CECommandList::SetShaderResourceView(int32_t rootParameterIndex, uint32_t descriptorOffset,
                                          const std::shared_ptr<CETexture>& texture, D3D12_RESOURCE_STATES stateAfter,
                                          UINT firstSubResource,
                                          UINT numSubResources) {
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                           const std::shared_ptr<CEUnorderedAccessView>& uav,
                                           D3D12_RESOURCE_STATES stateAfter, UINT firstSubResource,
                                           UINT numSubResources) {
}

void CECommandList::SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
                                           const std::shared_ptr<CETexture>& texture, UINT mip,
                                           D3D12_RESOURCE_STATES stateAfter, UINT firstSubResource,
                                           UINT numSubResources) {
}

void CECommandList::SetRenderTarget(const CERenderTarget& renderTarget) {
}

void CECommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex,
                         uint32_t startInstance) {
}

void CECommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex,
                                int32_t baseVertex, uint32_t startInstance) {
}

void CECommandList::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) {
}

CECommandList::CECommandList(CEDevice& device, D3D12_COMMAND_LIST_TYPE type) : m_device(device),
                                                                               m_commandListType(type),
                                                                               m_rootSignature(nullptr),
                                                                               m_pipelineState(nullptr) {
	auto d3d12Device = m_device.GetDevice();

	ThrowIfFailed(d3d12Device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(d3d12Device->CreateCommandList(0, m_commandListType, m_commandAllocator.Get(), nullptr,
	                                             IID_PPV_ARGS(&m_commandList)));
	m_uploadBuffer = std::make_unique<CEUploadBufferInstance>(device);
	m_resourceStateTracker = std::make_unique<CEResourceStateTracker>();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamicDescriptorHeap[i] = std::make_unique<CEDynamicDescriptorHeap>(
			device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		m_descriptorHeaps[i] = nullptr;
	}
}

CECommandList::~CECommandList() {
}

bool CECommandList::Close(const std::shared_ptr<CECommandList>& pendingCommandList) {
}

void CECommandList::Close() {
}

void CECommandList::Reset() {
}

void CECommandList::ReleaseTrackedObjects() {
}

void CECommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap) {
}

std::shared_ptr<CEScene> CECommandList::CreateScene(const VertexCollection& vertices,
                                                    const IndexCollection& indices) {
}

void CECommandList::CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices,
                                      size_t tessellation, float height, float radius, bool isTop) {
}

void CECommandList::TrackResource(wrl::ComPtr<ID3D12Object> object) {
}

void CECommandList::TrackResource(const std::shared_ptr<CEResource>& res) {
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

	/*
	 * Create SRV that uses format of original texture.
	 */
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = isSRGB ? CETexture::GetSRGBFormat(resourceDesc.Format) : resourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	auto srv = m_device.CreateShaderResourceView(texture, &srvDesc);
	for (uint32_t sourceMip = 0; sourceMip < resourceDesc.MipLevels - 1u;) {
		uint64_t sourceWidth = resourceDesc.Width >> sourceMip;
		uint32_t sourceHeight = resourceDesc.Height >> sourceMip;
		uint32_t destinationWidth = static_cast<uint32_t>(sourceWidth >> 1);
		uint32_t destinationHeight = sourceHeight >> 1;

		/*
		 * 0b00(0): Both width and height are even;
		 * 0b01(1): Both width and height are even;
		 * 0b10(2): Both width and height are even;
		 * 0b11(3): Both width and height are even;
		 */
		generateMipsCB.SrcDimension = (sourceHeight & 1) << 1 | (sourceWidth & 1);

		/*
		 * How many mipmap levels to compute this pass (max 4 mips per pass)
		 */
		DWORD mipCount;

		/*
		 * Number of times texture can be half size and get exactly 50% reduction in size.
		 * 1 bit in width or height indicates an odd dimension.
		 * Case where either width or height is exactly 1 is handled as special case
		 * (as dimension does not require reduction).
		 */
		_BitScanForward(&mipCount,
		                (destinationWidth == 1 ? destinationHeight : destinationWidth)
		                | (destinationHeight == 1 ? destinationWidth : destinationHeight)
		);
		/*
		 * Maximum number of mips to generate is 4
		 */
		mipCount = std::min<DWORD>(4, mipCount + 1);
		/*
		 * Clamp to total number of mips left over.
		 */
		mipCount = (sourceMip + mipCount) >= resourceDesc.MipLevels ? resourceDesc.MipLevels - sourceMip - 1 : mipCount;

		/*
		 * Dimensions should not reduce to 0
		 * can happen if width and height are not same.
		 */
		destinationWidth = std::max<DWORD>(1, destinationWidth);
		destinationHeight = std::max<DWORD>(1, destinationHeight);

		generateMipsCB.SrcMipLevel = sourceMip;
		generateMipsCB.NumMipLevels = mipCount;
		generateMipsCB.TexelSize.x = 1.0f / (float)destinationWidth;
		generateMipsCB.TexelSize.y = 1.0f / (float)destinationHeight;

		SetCompute32BitConstants(GenerateMips::GenerateMipsCB, generateMipsCB);

		SetShaderResourceView(GenerateMips::SrcMip, 0, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, sourceMip,
		                      1);

		for (uint32_t mip = 0; mip < mipCount; ++mip) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = sourceMip + mip + 1;

			auto uav = m_device.CreateUnorderedAccessView(texture, nullptr, &uavDesc);
			SetUnorderedAccessView(GenerateMips::OutMip, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			                       sourceMip + mip + 1, 1);
		}

		/*
		 * Pad any unused mip levels with a default UAV.
		 */
		if (mipCount < 4) {
			m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
				GenerateMips::OutMip, mipCount, 4 - mipCount, m_generateMipsPSO->GetDefaultUAV());
		}

		Dispatch(Concept::Math::DivideByMultiple(destinationWidth, 8),
		         Concept::Math::DivideByMultiple(destinationHeight, 8));

		UAVBarrier(texture);

		sourceMip += mipCount;
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> CECommandList::CopyBuffer(size_t bufferSize, const void* bufferData,
                                                                 D3D12_RESOURCE_FLAGS flags) {
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
	if (bufferSize != 0) {
		auto d3d12Device = m_device.GetDevice();
		ThrowIfFailed(d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
			D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&d3d12Resource)
		));

		/*
		 * Add resource to global resource state tracker;
		 */
		CEResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		if (bufferData != nullptr) {
			/*
			 * Create umplad resource to use as intermediate buffer to copy buffer resource
			 */
			Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
			ThrowIfFailed(d3d12Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&uploadResource)
			));

			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = bufferData;
			subResourceData.RowPitch = bufferSize;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			m_resourceStateTracker->TransitionResource(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();
			UpdateSubresources(m_commandList.Get(), d3d12Resource.Get(), uploadResource.Get(), 0, 0, 1,
			                   &subResourceData);

			/*
			 * Add references to resources for keep them in scope until reset of command list.
			 */
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	};

	return d3d12Resource;
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
