#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <QWidget>
#include <QTimer>
#include <stdexcept>

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
		void DeviceInitialized(bool success);

		void EventHandled();
		void WidgetResized();

		void Updated();
		void Rendered(ID3D12GraphicsCommandList* commandList);

		void KeyPressed(QKeyEvent*);
		void MouseMoved(QMouseEvent*);
		void MouseClicked(QMouseEvent*);
		void MouseReleased(QMouseEvent*);

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

		D3DCOLORVALUE* ClearColor() { &m_clearColor; };

	private:
		static int const FRAME_COUNT = 3;
		UINT m_currFrameIndex;

		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_commandAllocators[FRAME_COUNT];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescHeap;

		UINT m_RTVDescSize;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_RTVResources[FRAME_COUNT];
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RTVDescriptors[FRAME_COUNT];

		HANDLE m_swapChainEvent;
		HANDLE m_fenceEvent;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		std::vector<UINT64> m_fenceValues[FRAME_COUNT];

		QTimer m_qTimer;

		HWND m_hWnd;
		bool m_deviceInitialized;

		bool m_renderActive;
		bool m_started;

		D3DCOLORVALUE m_clearColor;
	};
}
