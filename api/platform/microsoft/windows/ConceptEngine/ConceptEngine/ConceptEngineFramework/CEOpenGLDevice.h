#pragma once
#include "CEDevice.h"

class CEOpenGLDevice : public CEDevice {

public:
	CEOpenGLDevice(const std::shared_ptr<CEAdapter>& adapter)
		: CEDevice(adapter) {
	}

	virtual ~CEOpenGLDevice() {
	}

	std::shared_ptr<CEDevice> CreateDevice(std::shared_ptr<CEAdapter>) override;
	std::shared_ptr<CESwapChain> CreateSwapChain() override;
	std::shared_ptr<CEGUI> CreateGUI() override;
	std::shared_ptr<CEConstantBuffer> CreateConstantBuffer() override;
	std::shared_ptr<CEByteAddressBuffer> CreateByteAddressBuffer(size_t bufferSize) override;
	std::shared_ptr<CEStructuredBuffer> CreateStructuredBuffer(size_t numElements, size_t elementSize) override;
	std::shared_ptr<CETexture> CreateTexture(CEResourceDescriptor resourceDesc) override;
	std::shared_ptr<CEIndexBuffer> CreateIndexBuffer() override;
	std::shared_ptr<CEVertexBuffer> CreateVertexBuffer() override;
	std::shared_ptr<CERootSignature> CreateRootSignature() override;
	CEDescriptorAllocation AllocateDescriptors(CEType type, uint32_t numDescriptors) override;
	std::shared_ptr<CEPipelineState> CreatePipelineState() override;
	std::shared_ptr<CEConstantBufferView> CreateConstantBufferView(
		const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset) override;
	std::shared_ptr<CEShaderResourceView>
	CreateShaderResourceView(const std::shared_ptr<CEResource>& resource) override;
	std::shared_ptr<CEUnorderedAccessView> CreateUnorderedAccessView(const std::shared_ptr<CEResource>& resource,
		const std::shared_ptr<CEResource>& counterResource) override;
	void Flush() override;
	void ReleaseStaleDescriptors() override;
	std::wstring GetDescription() const override;
	std::string GetHighestRootSignatureVersion() const override;
};
