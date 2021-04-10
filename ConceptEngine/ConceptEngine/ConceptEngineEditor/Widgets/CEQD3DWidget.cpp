#include "CEQD3DWidget.h"

#include <QtWidgets/QMainWindow>

using namespace ConceptEngine::Editor::Widgets;

CEQD3DWidget::CEQD3DWidget(QWidget* parent): QWidget(parent) {
	m_hWnd = reinterpret_cast<HWND>(winId());
}

CEQD3DWidget::~CEQD3DWidget() {
}

void CEQD3DWidget::Release() {
}

void CEQD3DWidget::ResetEnvironment() {
}

void CEQD3DWidget::Run() {
}

void CEQD3DWidget::PauseFrames() {
}

void CEQD3DWidget::ContinueFrames() {
}

bool CEQD3DWidget::Create() {
	return true;
}

bool CEQD3DWidget::CreateD3DDevice() {
	return true;
}

void CEQD3DWidget::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
}

void CEQD3DWidget::Resize(int width, int height) {
}

void CEQD3DWidget::ClearRenderTarget() {
}

void CEQD3DWidget::CreateRenderTarget() {
}

void CEQD3DWidget::BeginScene() {
}

void CEQD3DWidget::EndScene() {
}

void CEQD3DWidget::Update() {
}

void CEQD3DWidget::Render() {
}

void CEQD3DWidget::WaitForGPU() {
}

void CEQD3DWidget::MoveToNextFrame() {
}

bool CEQD3DWidget::event(QEvent* event) {
	return false;
}

void CEQD3DWidget::showEvent(QShowEvent* event) {
}

QPaintEngine* CEQD3DWidget::paintEngine() const {
	return nullptr;
}

void CEQD3DWidget::paintEvent(QPaintEvent* event) {
}

void CEQD3DWidget::resizeEvent(QResizeEvent* event) {
}

void CEQD3DWidget::wheelEvent(QWheelEvent* event) {
}

LRESULT CEQD3DWidget::WndProc(MSG* pMsg) {

	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == 0x020A) return false;

	return false;
}

bool CEQD3DWidget::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	return false;
}

void CEQD3DWidget::OnFrame() {
}

void CEQD3DWidget::OnReset() {
}
