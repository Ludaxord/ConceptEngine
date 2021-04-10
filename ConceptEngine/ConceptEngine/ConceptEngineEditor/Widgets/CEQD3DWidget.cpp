#include "CEQD3DWidget.h"

using namespace ConceptEngine::Editor::Widgets;

const QMetaObject* CEQD3DWidget::metaObject() const {
}

void* CEQD3DWidget::qt_metacast(const char*) {
}

int CEQD3DWidget::qt_metacall(QMetaObject::Call, int, void**) {
}

void CEQD3DWidget::qt_static_metacall(QObject*, QMetaObject::Call, int, void**) {
}

CEQD3DWidget::CEQD3DWidget(QWidget* parent) {
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
}

bool CEQD3DWidget::CreateD3DDevice() {
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
}

void CEQD3DWidget::showEvent(QShowEvent* event) {
}

QPaintEngine* CEQD3DWidget::paintEngine() const {
}

void CEQD3DWidget::paintEvent(QPaintEvent* event) {
}

void CEQD3DWidget::resizeEvent(QResizeEvent* event) {
}

void CEQD3DWidget::wheelEvent(QWheelEvent* event) {
}

LRESULT CEQD3DWidget::WndProc(MSG* pMsg) {
}

bool CEQD3DWidget::nativeEvent(const QByteArray& eventType, void* message, long* result) {
}

void CEQD3DWidget::DeviceInitialized(bool success) {
}

void CEQD3DWidget::EventHandled() {
}

void CEQD3DWidget::WidgetResized() {
}

void CEQD3DWidget::Updated() {
}

void CEQD3DWidget::Rendered(ID3D12GraphicsCommandList* commandList) {
}

void CEQD3DWidget::KeyPressed(QKeyEvent*) {
}

void CEQD3DWidget::MouseMoved(QMouseEvent*) {
}

void CEQD3DWidget::MouseClicked(QMouseEvent*) {
}

void CEQD3DWidget::MouseReleased(QMouseEvent*) {
}

void CEQD3DWidget::OnFrame() {
}

void CEQD3DWidget::OnReset() {
}
