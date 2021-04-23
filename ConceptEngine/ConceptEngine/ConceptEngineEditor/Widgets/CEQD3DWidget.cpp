#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "CEQD3DWidget.h"

#include <QEvent>
#include <QtWidgets/QMainWindow>
#include <QDirect3D12Widget.h>

#include "../../ConceptEngineFramework/CEFramework.h"


using namespace ConceptEngine::Editor::Widgets;

CEQD3DWidget::CEQD3DWidget(QWidget* parent): QWidget(parent) {
	m_deviceCreated = false;
	m_renderActive = false;
	m_hWnd = reinterpret_cast<HWND>(winId());

	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_NativeWindow);

	// Setting these attributes to our widget and returning null on paintEngine event
	// tells Qt that we'll handle all drawing and updating the widget ourselves.
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
}

CEQD3DWidget::CEQD3DWidget(QWidget* parent, ConceptEngineFramework::Graphics::CEPlayground* playground): CEQD3DWidget(
	parent) {
	m_playground = playground;
}

CEQD3DWidget::~CEQD3DWidget() {
}

bool CEQD3DWidget::Create() {
	if (m_playground == nullptr) {
		//TODO: Generate CEEmptyPlayground ==> representing new project
		throw std::exception("Playground not found!");
	}
	m_framework = std::make_shared<ConceptEngineFramework::CEFramework>(
		m_hWnd,
		ConceptEngineFramework::Graphics::API::DirectX12_API,
		width(),
		height(),
		m_playground
	);

	return true;
}

void CEQD3DWidget::Run() {
}

void CEQD3DWidget::Release() {
}

void CEQD3DWidget::Pause() {
}

void CEQD3DWidget::Resume() {
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

			if (nativeParent && nativeParent != this &&
				::GetFocus() == reinterpret_cast<HWND>(nativeParent->winId()))
				::SetFocus(m_hWnd);
		}
		break;
	case QEvent::KeyPress:
		emit SignalKeyPressed(reinterpret_cast<QKeyEvent*>(event));
		break;
	case QEvent::MouseMove:
		emit SignalMouseMoved(reinterpret_cast<QMouseEvent*>(event));
		break;
	case QEvent::MouseButtonPress:
		emit SignalMouseClicked(reinterpret_cast<QMouseEvent*>(event));
		break;
	case QEvent::MouseButtonRelease:
		emit SignalMouseReleased(reinterpret_cast<QMouseEvent*>(event));
		break;
	default:
		break;
	}

	return QWidget::event(event);
}

void CEQD3DWidget::showEvent(QShowEvent* event) {
	if (!m_deviceCreated) {
		qDebug("Show Event: Concept Engine QDirect3D 12");
		m_deviceCreated = Create();
	}
	QWidget::showEvent(event);
}

QPaintEngine* CEQD3DWidget::paintEngine() const {
	return Q_NULLPTR;
}

void CEQD3DWidget::paintEvent(QPaintEvent* event) {
}

void CEQD3DWidget::resizeEvent(QResizeEvent* event) {
}

void CEQD3DWidget::wheelEvent(QWheelEvent* event) {
}


void CEQD3DWidget::Update() {
}

void CEQD3DWidget::Render() {
}


LRESULT CEQD3DWidget::WndProc(MSG* pMsg) {
	// Process wheel events using Qt's event-system.
	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_MOUSEHWHEEL) return false;

	return false;
}

void CEQD3DWidget::OnFrame() {
}

void CEQD3DWidget::OnReset() {
}

#if QT_VERSION >= 0x050000
bool CEQD3DWidget::nativeEvent(const QByteArray& eventType,
                               void* message,
                               long* result) {
	Q_UNUSED(eventType);
	Q_UNUSED(result);

#    ifdef Q_OS_WIN
	MSG* pMsg = reinterpret_cast<MSG*>(message);
	return WndProc(pMsg);
#    endif

	return QWidget::nativeEvent(eventType, message, result);
}

#else // QT_VERSION < 0x050000
bool CEQD3DWidget::winEvent(MSG * message, long * result)
{
    Q_UNUSED(result);

#    ifdef Q_OS_WIN
    MSG * pMsg = reinterpret_cast<MSG *>(message);
    return WndProc(pMsg);
#    endif

    return QWidget::winEvent(message, result);
}
#endif // QT_VERSION >= 0x050000
