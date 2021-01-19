#pragma once

#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXDevice;
	namespace wrl = Microsoft::WRL;

	class CEDirectXRootSignature {
	public:
		wrl::ComPtr<ID3D12RootSignature> GetD3D12RootSignature() const {
			return m_rootSignature;
		}

		const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const {
			return m_rootSignatureDesc;
		}

		uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
		uint32_t GetNumDescriptors(uint32_t rootIndex) const;

	protected:
		friend class std::default_delete<CEDirectXRootSignature>;
		CEDirectXRootSignature(CEDirectXRootSignature& device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);
		virtual ~CEDirectXRootSignature();

	private:
		void Destroy();
		void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

		CEDirectXDevice& m_device;
		D3D12_ROOT_SIGNATURE_DESC1 m_rootSignatureDesc;
		wrl::ComPtr<ID3D12RootSignature> m_rootSignature;

		/*
		 * Need to know number of descriptor per descriptor table.
		 * maximum of 32 descriptor tables are supported
		 * (since 32-bit mask is used to represent descriptor tables in root signature)
		 */
		static const int m_descriptorTablesMask = 32;
		uint32_t m_numDescriptorsPerTable[m_descriptorTablesMask];

		/*
		 * bit mask that represents root parameter indices that are descrptor tables for samplers;
		 */
		uint32_t m_samplerTableBitMask;

		/*
		 * bit mask that represent root parameter indices that are CBV, UAV and SRV descriptor tables.
		 */
		uint32_t m_descriptorTableBitMask;
	};
}
