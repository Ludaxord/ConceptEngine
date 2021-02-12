#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgi1_6.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

namespace ConceptFramework::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEAdapter;
	using AdapterList = std::vector<std::shared_ptr<CEAdapter>>;

	class CEAdapter {
	public:
		/**
		 * Get a list of DX12 compatible hardware adapters sorted by the GPU preference.
		 *
		 * @param gpuPreference The GPU preference to sort the returned adapters.
		 */
		static AdapterList GetAdapters(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);

		/**
		 * Create a GPU adapter.
		 *
		 * @param gpuPreference The GPU preference. By default, a high-performance
		 * GPU is preferred.
		 * @param useWarp If true, create a WARP adapter.
		 *
		 * @returns A shared pointer to a GPU adapter or nullptr if the adapter
		 * could not be created.
		 */
		static std::shared_ptr<CEAdapter> Create(
			DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			bool useWarp = false);

		/**
		 * Get the IDXGIAdapter
		 */
		Microsoft::WRL::ComPtr<IDXGIAdapter4> GetDXGIAdapter() const {
			return m_dxgiAdapter;
		}

		/**
		 * Get the description of the adapter.
		 */
		const std::wstring GetDescription() const;

		D3D_FEATURE_LEVEL GetFeatureLevel() const;
		std::string GetFeatureLevelName() const;

	protected:
		CEAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter);
		virtual ~CEAdapter() = default;

	private:
		void SetFeatureLevel(wrl::ComPtr<ID3D12Device> device);

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
		DXGI_ADAPTER_DESC3 m_desc;
		D3D_FEATURE_LEVEL m_d3dFeatureLevel;

		inline static D3D_FEATURE_LEVEL m_d3dMinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		inline static const D3D_FEATURE_LEVEL m_featureLevels[] = {
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
	};
}
