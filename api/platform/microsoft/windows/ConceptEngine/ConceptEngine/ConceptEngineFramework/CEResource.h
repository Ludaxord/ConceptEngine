#pragma once
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEResource {

	public:
		/*
		 * Get device that was used to create resource.
		 */
		CEDevice& GetDevice() const {
			return m_device;
		}

		/*
		 * Get access to underlying D3D12 resource
		 */
		wrl::ComPtr<ID3D12Resource> GetD3D12Resource() const {
			return m_resource;
		}

		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const {
			D3D12_RESOURCE_DESC resDesc = {};
			if (m_resource) {
				resDesc = m_resource->GetDesc();
			}
			return resDesc;
		}

		/*
		 * Set name of resource. Useful in debugging
		 */
		void SetName(const std::wstring& name);

		const std::wstring& GetName() {
			return m_resourceName;
		}

		/*
		 * Check if resource format supports a specific feature.
		 */
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

	protected:
		/*
		 * Resource creation should go through device
		 */
		CEResource(CEDevice& device, const D3D12_RESOURCE_DESC& resourceDesc,
		                  const D3D12_CLEAR_VALUE* clearValue = nullptr);
		CEResource(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource,
		                  const D3D12_CLEAR_VALUE* clearValue = nullptr);
		virtual ~CEResource() = default;

		/*
		 * Device that is used to create resource
		 */
		CEDevice& m_device;

		/*
		 * Underlying D3D12 Resource;
		 */
		wrl::ComPtr<ID3D12Resource> m_resource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT m_formatSupport;
		std::unique_ptr<D3D12_CLEAR_VALUE> m_clearValue;
		std::wstring m_resourceName;

	private:
		/*
		 * Check format support and populate m_formatSupport Struct
		 */
		void CheckFeatureSupport();
	};
}
