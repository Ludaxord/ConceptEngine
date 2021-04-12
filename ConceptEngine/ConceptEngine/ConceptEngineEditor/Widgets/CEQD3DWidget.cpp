#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "CEQD3DWidget.h"


#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>


#include "../../ConceptEngineFramework/Graphics/DirectX12/Libraries/d3dx12.h"

using namespace ConceptEngine::Editor::Widgets;

constexpr int FPS_LIMIT = 60.0f;
constexpr int MS_PER_FRAME = (int)((1.0f / FPS_LIMIT) * 1000.0f);

CEQD3DWidget::CEQD3DWidget(QWidget* parent): QWidget(parent) {
	m_hWnd = reinterpret_cast<HWND>(winId());

	qDebug() << "[CEQD3DWidget::CEQD3DWidget] - Widget Handle: " << m_hWnd;

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::gray);
	setAutoFillBackground(true);
	setPalette(palette);

	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_NativeWindow);

	/*
	 * Setting there attributes to widget and returning null on paintEngine event
	 * tells Qt that drawing and updating will be handled by ourselves
	 */
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
}

CEQD3DWidget::~CEQD3DWidget() {
}

void CEQD3DWidget::Release() {
	m_deviceInitialized = false;
	disconnect(&m_qTimer, &QTimer::timeout, this, &CEQD3DWidget::OnFrame);
	m_qTimer.stop();

	WaitForGPU();

	for (UINT i = 0; i < FRAME_COUNT; i++) {
		ReleaseObject(m_RTVResources[i])
	}
	ReleaseObject(m_swapChain);
	ReleaseHandle(m_swapChainEvent);
	ReleaseObject(m_commandQueue);
	for (UINT i = 0; i < FRAME_COUNT; i++) {
		ReleaseObject(m_commandAllocators[i]);
	}
	ReleaseObject(m_commandList);
	ReleaseObject(m_RTVDescHeap);
	ReleaseObject(m_SRVDescHeap);
	ReleaseObject(m_fence);
	ReleaseHandle(m_fenceEvent);
	ReleaseObject(m_d3dDevice);
	ReleaseObject(m_factory);
}

void CEQD3DWidget::ResetEnvironment() {
	OnReset();

	if (!m_renderActive)
		Update();
}

void CEQD3DWidget::Run() {
	m_qTimer.start(MS_PER_FRAME);
	m_renderActive = m_started = true;
}

void CEQD3DWidget::PauseFrames() {
	if (!m_qTimer.isActive() || !m_started) return;
	disconnect(&m_qTimer, &QTimer::timeout, this, &CEQD3DWidget::OnFrame);
	m_qTimer.stop();
	m_renderActive = false;
}

void CEQD3DWidget::ContinueFrames() {
	if (m_qTimer.isActive() || m_started) return;
	connect(&m_qTimer, &QTimer::timeout, this, &CEQD3DWidget::OnFrame);
	m_qTimer.start(MS_PER_FRAME);
	m_renderActive = true;
}

bool CEQD3DWidget::Create() {
	CreateD3DDevice();
	ResetEnvironment();
	connect(&m_qTimer, &QTimer::timeout, this, &CEQD3DWidget::OnFrame);

	return true;
}

void CEQD3DWidget::CreateD3DDevice() {
	UINT factoryFlags = 0;
	bool debugDXGI = false;

#if defined(DEBUG) || defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
		debugDXGI = true;
		DXCall(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_factory)));

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	if (!debugDXGI) {
		CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	GetHardwareAdapter(m_factory.Get(), adapter.GetAddressOf());
	if (!adapter)
		DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));

	DXCall(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0 , IID_PPV_ARGS(&m_d3dDevice)));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DXCall(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.BufferCount = FRAME_COUNT;
		sd.Width = width();
		sd.Height = height();
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.Stereo = FALSE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSd = {};
		fsSd.Windowed = TRUE;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
		DXCall(
			m_factory->CreateSwapChainForHwnd(
				m_commandQueue.Get(), m_hWnd, &sd, &fsSd, nullptr, swapChain.GetAddressOf()
			)
		);
		DXCall(swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain)));
		m_currFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.NumDescriptors = FRAME_COUNT;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DXCall(m_d3dDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_RTVDescHeap)));
	m_RTVDescSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < FRAME_COUNT; i++) {
		m_RTVDescriptors.push_back(rtvHandle);
		DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_RTVResources[i])));
		m_d3dDevice->CreateRenderTargetView(m_RTVResources[i].Get(), nullptr, m_RTVDescriptors[i]);
		rtvHandle.Offset(1, m_RTVDescSize);
	}

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 1;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXCall(m_d3dDevice->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&m_SRVDescHeap)));

	for (UINT i = 0; i < FRAME_COUNT; i++) {
		DXCall(
			m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
	}

	DXCall(
		m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_currFrameIndex].Get(),
			nullptr, IID_PPV_ARGS(&m_commandList)));
	DXCall(m_commandList->Close());

	DXCall(m_d3dDevice->CreateFence(m_fenceValues[m_currFrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValues[m_currFrameIndex]++;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_fenceEvent)
		DXCall(HRESULT_FROM_WIN32(GetLastError()));

	WaitForGPU();
}

void CEQD3DWidget::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0;
	     DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter);
	     ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
			break;
		}
	}
	*ppAdapter = adapter.Detach();
}

void CEQD3DWidget::Resize(int width, int height) {
	ClearRenderTarget();

	if (m_swapChain) {
		DXCall(m_swapChain->ResizeBuffers(FRAME_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	}
	else {
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.BufferCount = FRAME_COUNT;
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.Stereo = FALSE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSd = {};
		fsSd.Windowed = TRUE;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
		DXCall(
			m_factory->CreateSwapChainForHwnd(
				m_commandQueue.Get(), m_hWnd, &sd, &fsSd, nullptr, swapChain.GetAddressOf()
			)
		);
		DXCall(swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain)));
		m_currFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	CreateRenderTarget();

	m_currFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void CEQD3DWidget::ClearRenderTarget() {
	WaitForGPU();

	for (UINT i = 0; i < FRAME_COUNT; i++) {
		ReleaseObject(m_RTVResources[i]);
		m_fenceValues[i] = m_fenceValues[m_currFrameIndex];
	}
}

void CEQD3DWidget::CreateRenderTarget() {
	for (UINT i = 0; i < FRAME_COUNT; i++) {
		DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_RTVResources[i])));
		m_d3dDevice->CreateRenderTargetView(m_RTVResources[i].Get(), nullptr, m_RTVDescriptors[i]);
	}
}

void CEQD3DWidget::BeginScene() {
	DXCall(m_commandAllocators[m_currFrameIndex]->Reset());
	DXCall(m_commandList->Reset(m_commandAllocators[m_currFrameIndex].Get(), nullptr));

	auto transitionPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(m_RTVResources[m_currFrameIndex].Get(),
	                                                                          D3D12_RESOURCE_STATE_PRESENT,
	                                                                          D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	m_commandList->ResourceBarrier(1, &transitionPresentRenderTarget);
}

void CEQD3DWidget::EndScene() {
	auto transitionRenderTargetPresent = CD3DX12_RESOURCE_BARRIER::Transition(m_RTVResources[m_currFrameIndex].Get(),
	                                                                          D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                                          D3D12_RESOURCE_STATE_PRESENT
	);
	m_commandList->ResourceBarrier(1, &transitionRenderTargetPresent);
	DXCall(m_commandList->Close());
	auto cmdList = m_commandList.Get();
	m_commandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&cmdList));
	DXCall(m_swapChain->Present(1, 0));
	MoveToNextFrame();
}

void CEQD3DWidget::Update() {
	//TODO: UPDATE SCENE HERE....

	emit ticked();

}

void CEQD3DWidget::Render() {
	m_commandList->ClearRenderTargetView(m_RTVDescriptors[m_currFrameIndex],
	                                     reinterpret_cast<const float*>(&m_clearColor), 0, nullptr);
	m_commandList->OMSetRenderTargets(1, &m_RTVDescriptors[m_currFrameIndex], FALSE, nullptr);
	m_commandList->SetDescriptorHeaps(1, &m_SRVDescHeap);

	emit rendered(m_commandList.Get());
}

void CEQD3DWidget::WaitForGPU() {
	DXCall(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_currFrameIndex]));

	DXCall(m_fence->SetEventOnCompletion(m_fenceValues[m_currFrameIndex], m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);
	m_fenceValues[m_currFrameIndex]++;
}

void CEQD3DWidget::MoveToNextFrame() {
	const UINT64 currentFenceValue = m_fenceValues[m_currFrameIndex];
	DXCall(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

	m_currFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
	if (m_fence->GetCompletedValue() < m_fenceValues[m_currFrameIndex]) {
		DXCall(m_fence->SetEventOnCompletion(m_fenceValues[m_currFrameIndex], m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_fenceValues[m_currFrameIndex] = currentFenceValue + 1;
}

bool CEQD3DWidget::event(QEvent* event) {
	switch (event->type()) {
		// Workaround for https://bugreports.qt.io/browse/QTBUG-42183 to get key strokes.
		// To make sure that we always have focus on the widget when we enter the rect area.
	case QEvent::Enter:
	case QEvent::FocusIn:
	case QEvent::FocusAboutToChange:
		if (::GetFocus() != m_hWnd) {
			QWidget* nativeParent = this;
			while (true) {
				if (nativeParent->isWindow()) break;
				QWidget* parent = nativeParent->nativeParentWidget();
				if (!parent) break;
				nativeParent = parent;
			}

			if (nativeParent && nativeParent != this && ::GetFocus() == reinterpret_cast<HWND>(nativeParent->winId()))
				::SetFocus(m_hWnd);
		}
		break;
	case QEvent::KeyPress:
		emit keyPressed((QKeyEvent*)event);
		break;
	case QEvent::MouseMove:
		emit mouseMoved((QMouseEvent*)event);
		break;
	case QEvent::MouseButtonPress:
		emit mouseClicked((QMouseEvent*)event);
		break;
	case QEvent::MouseButtonRelease:
		emit mouseReleased((QMouseEvent*)event);
		break;
	}

	return QWidget::event(event);
}

void CEQD3DWidget::showEvent(QShowEvent* event) {
	if (m_deviceInitialized) {
		m_deviceInitialized = Create();
		emit deviceInitialized(m_deviceInitialized);
	}

	QWidget::showEvent(event);
}

QPaintEngine* CEQD3DWidget::paintEngine() const {
	return nullptr;
}

void CEQD3DWidget::paintEvent(QPaintEvent* event) {
}

void CEQD3DWidget::resizeEvent(QResizeEvent* event) {
	if (m_deviceInitialized) {
		OnReset();
		emit widgetResized();
	}

	QWidget::resizeEvent(event);
}

void CEQD3DWidget::wheelEvent(QWheelEvent* event) {
	if (event->angleDelta().x() == 0) {
	}
	else if (event->angleDelta().x() != 0) {
	}
	else if (event->angleDelta().y() != 0) {
	}

	QWidget::wheelEvent(event);
}

LRESULT CEQD3DWidget::WndProc(MSG* pMsg) {

	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == 0x020A) return false;

	return false;
}

#if QT_VERSION >= 0x050000
bool CEQD3DWidget::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	Q_UNUSED(eventType);
	Q_UNUSED(result);

#if defined(Q_OS_WIN)
	MSG* pMsg = reinterpret_cast<MSG*>(message);
	return WndProc(pMsg);
#endif

	return QWidget::nativeEvent(eventType, message, result);
}

#else
bool CEQD3DWidget::winEvent(MSG* message, long* result) {
	Q_UNUSED(result);

#    ifdef Q_OS_WIN
	MSG* pMsg = reinterpret_cast<MSG*>(message);
	return WndProc(pMsg);
#    endif

	return QWidget::winEvent(message, result);
}
#endif

void CEQD3DWidget::OnFrame() {
	if (m_renderActive)
		Update();

	BeginScene();
	Render();
	EndScene();
}

void CEQD3DWidget::OnReset() {
	// DirectX-12 samples here: https://github.com/microsoft/DirectX-Graphics-Samples how to
	// properly do this without leaking memory.
	PauseFrames();
	Resize(width(), height());
	ContinueFrames();
}
