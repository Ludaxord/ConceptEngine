#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdexcept>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

#include <wrl/client.h>

namespace ConceptEngine::Editor::Widgets {

#define ReleaseObject(object)                                                                 \
    if ((object) != Q_NULLPTR)                                                                \
    {                                                                                         \
        object->Release();                                                                    \
        object = Q_NULLPTR;                                                                   \
    }
#define ReleaseHandle(object)                                                                 \
    if ((object) != Q_NULLPTR)                                                                \
    {                                                                                         \
        CloseHandle(object);                                                                  \
        object = Q_NULLPTR;                                                                   \
    }

	inline std::string HrToString(HRESULT hr) {
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}

	class HrException : public std::runtime_error {
	public:
		HrException(HRESULT hr): std::runtime_error(HrToString(hr)), m_hr(hr) {
		}

		HRESULT Error() const { return m_hr; }

	private:
		const HRESULT m_hr;
	};

	inline void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr)) { throw HrException(hr); }
	}

#define DXCall(func) ThrowIfFailed(func)


	class CEQD3DWidget : public QWidget {
	Q_OBJECT

	public:
		CEQD3DWidget(QWidget* parent);
		~CEQD3DWidget();

		void Release();
		void ResetEnvironment();

		void Run();
		void PauseFrames();
		void ContinueFrames();
	protected:
	private:
		bool Create();

		/*
		 * TODO: for test purposes create all Direct3D logic in Widget,
		 * TODO: after successful initialization of QT use prepared Direct3D logic in ConceptEngineFramework project
		 */
		bool CreateD3DDevice();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void Resize(int width, int height);
		void ClearRenderTarget();
		void CreateRenderTarget();

		void BeginScene();
		void EndScene();

		void Update();
		void Render();

		void WaitForGPU();
		void MoveToNextFrame();

		bool event(QEvent* event) override;
		void showEvent(QShowEvent* event) override;
		QPaintEngine* paintEngine() const override;
		void paintEvent(QPaintEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;

		LRESULT WINAPI WndProc(MSG* pMsg);


#if QT_VERSION >= 0x050000
		bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#else
		bool winEvent(MSG* message, long* result) override;
#endif

	signals:
		void deviceInitialized(bool success);

		void eventHandled();
		void widgetResized();

		void ticked();
		void rendered(ID3D12GraphicsCommandList* cl);
		void renderedUI();

		void keyPressed(QKeyEvent*);
		void mouseMoved(QMouseEvent*);
		void mouseClicked(QMouseEvent*);
		void mouseReleased(QMouseEvent*);

	private slots:
		void OnFrame();
		void OnReset();

	public:
		HWND const& GetWindowHandle() { return m_hWnd; };

		Microsoft::WRL::ComPtr<ID3D12Device> GetD3DDevice() const { return m_d3dDevice; };
		Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; };
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return m_commandList; };

		bool RenderActive() const { return m_renderActive; };
		void SetRenderActive(bool active) { m_renderActive = active; };

		D3DCOLORVALUE* ClearColor() { return &m_clearColor; };

	private:
		static int const FRAME_COUNT = 3;
		UINT m_currFrameIndex = 0;

		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_commandAllocators[FRAME_COUNT] = {};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescHeap = nullptr;

		UINT m_RTVDescSize = 0;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_RTVResources[FRAME_COUNT] = {};
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RTVDescriptors[FRAME_COUNT] = {};

		HANDLE m_swapChainEvent = nullptr;
		HANDLE m_fenceEvent = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence = nullptr;
		std::vector<UINT64> m_fenceValues[FRAME_COUNT] = {};

		QTimer m_qTimer;

		HWND m_hWnd;
		bool m_deviceInitialized = false;

		bool m_renderActive = false;
		bool m_started = false;

		D3DCOLORVALUE m_clearColor = {0.0f, 0.135f, 0.481f, 1.0f};
	};
}
