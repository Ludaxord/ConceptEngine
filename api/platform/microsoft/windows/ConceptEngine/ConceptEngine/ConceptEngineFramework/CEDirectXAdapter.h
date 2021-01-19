#pragma once
#include <dxgi1_6.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	namespace wrl = Microsoft::WRL;

	class CEDirectXAdapter;
	using CEDirectXAdapterList = std::vector<std::shared_ptr<CEDirectXAdapter>>;

	class CEDirectXAdapter {
	public:
		/*
		 * Get list of compatible hardware adapters sorted by GPU performance;
		 *
		 * @param gpuPreference - gpu preference to sort returned adapters;
		 */
		static CEDirectXAdapterList GetAdapters(
			DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);

		/*
		 * Create GPU Adapter
		 * @param gpuPreference by default: High Performance GPU
		 * @param iseWarp, if true create WARP adapter.
		 *
		 * @returns shared pointer to GPU adapter or nullptr if adapter could not be created;
		 */
		static std::shared_ptr<CEDirectXAdapter> Create(
			DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, bool useWarp = false);

		/*
		 * Get IDXGIAdapter
		 */
		wrl::ComPtr<IDXGIAdapter4> GetDXGIAdapter() const {
			return m_dxgiAdapter;
		};

		/*
		 * Get adapter Description;
		 */
		const std::wstring GetDescription() const;

	protected:
		/*
		* Create Default Constructors/Destructors
		*/
		CEDirectXAdapter(wrl::ComPtr<IDXGIAdapter4> dxgiAdapter);
		virtual ~CEDirectXAdapter() = default;

	private:
		wrl::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
		DXGI_ADAPTER_DESC3 m_Desc;

	};
}
