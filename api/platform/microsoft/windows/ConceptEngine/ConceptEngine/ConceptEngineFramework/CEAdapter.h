#pragma once
#include <dxgi1_6.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

    class CEAdapter;
    using AdapterList = std::vector<std::shared_ptr<CEAdapter>>;

    class CEAdapter
    {
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
        static std::shared_ptr<CEAdapter> Create(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            bool                useWarp = false);

        /**
         * Get the IDXGIAdapter
         */
        Microsoft::WRL::ComPtr<IDXGIAdapter4> GetDXGIAdapter() const
        {
            return m_dxgiAdapter;
        }

        /**
         * Get the description of the adapter.
         */
        const std::wstring GetDescription() const;

    protected:
        CEAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter);
        virtual ~CEAdapter() = default;

    private:
        Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
        DXGI_ADAPTER_DESC3                    m_desc;
    };
}
