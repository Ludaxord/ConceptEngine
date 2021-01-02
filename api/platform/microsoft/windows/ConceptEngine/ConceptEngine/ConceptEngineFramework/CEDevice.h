#pragma once
#include <magic_enum.hpp>
#include <memory>
#include <string>


#include "CEAdapter.h"
#include "CEByteAddressBuffer.h"
#include "CECommandQueue.h"
#include "CEConstantBuffer.h"
#include "CEConstantBufferView.h"
#include "CEDescriptorAllocation.h"
#include "CEGraphics.h"
#include "CEGUI.h"
#include "CEIndexBuffer.h"
#include "CEPipelineState.h"
#include "CEResource.h"
#include "CEResourceDescriptor.h"
#include "CERootSignature.h"
#include "CEShaderResourceView.h"
#include "CEStructuredBuffer.h"
#include "CESwapChain.h"
#include "CETexture.h"
#include "CEType.h"
#include "CEUnorderedAccessView.h"
#include "CEVertexBuffer.h"

class CEDevice {

	using CEGraphicsApiType = CEOSTools::CEGraphicsApiTypes;
public:
	static std::shared_ptr<CEDevice> Create(std::shared_ptr<CEAdapter> adapter, CEGraphicsApiType apiTypes);
public:
	virtual std::shared_ptr<CEDevice> CreateDevice(std::shared_ptr<CEAdapter>  = nullptr) = 0;
	virtual std::shared_ptr<CESwapChain> CreateSwapChain() = 0;
	virtual std::shared_ptr<CEGUI> CreateGUI() = 0;
	virtual std::shared_ptr<CEConstantBuffer> CreateConstantBuffer() = 0;
	virtual std::shared_ptr<CEByteAddressBuffer> CreateByteAddressBuffer(size_t bufferSize) = 0;
	virtual std::shared_ptr<CEStructuredBuffer> CreateStructuredBuffer(size_t numElements, size_t elementSize) = 0;
	virtual std::shared_ptr<CETexture> CreateTexture(CEResourceDescriptor resourceDesc) = 0;
	virtual std::shared_ptr<CEIndexBuffer> CreateIndexBuffer() = 0;
	virtual std::shared_ptr<CEVertexBuffer> CreateVertexBuffer() = 0;
	virtual std::shared_ptr<CERootSignature> CreateRootSignature() = 0;
	virtual CEDescriptorAllocation AllocateDescriptors(CEType type, uint32_t numDescriptors) = 0;
	virtual std::shared_ptr<CEPipelineState> CreatePipelineState() = 0;
	virtual std::shared_ptr<CEConstantBufferView> CreateConstantBufferView(
		const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset = 0) = 0;
	virtual std::shared_ptr<CEShaderResourceView> CreateShaderResourceView(const std::shared_ptr<CEResource>& resource)
	= 0;
	virtual std::shared_ptr<CEUnorderedAccessView> CreateUnorderedAccessView(
		const std::shared_ptr<CEResource>& resource, const std::shared_ptr<CEResource>& counterResource = nullptr) = 0;

	virtual void Flush() = 0;
	virtual void ReleaseStaleDescriptors() = 0;

	virtual std::wstring GetDescription() const = 0;

	std::shared_ptr<CEAdapter> GetAdapter() const {
		return adapter_;
	}

	virtual CECommandQueue& GetCommandQueue(CEType type);

	virtual std::string GetHighestRootSignatureVersion() const = 0;

protected:
	explicit CEDevice(std::shared_ptr<CEAdapter> adapter);
	virtual ~CEDevice() = default;
private:
	std::shared_ptr<CEAdapter> adapter_;
	CERootSignatureVersion rootSignatureVersion_;
};
