#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "CEQD3DWidget.h"

#include <QEvent>
#include <QtWidgets/QMainWindow>
#include <QDirect3D12Widget.h>
#include <qevent.h>

#include "../../ConceptEngineFramework/CEFramework.h"
#include "../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Editor::Widgets;
using namespace ConceptEngineFramework::Graphics;

constexpr int FPS_LIMIT = 60.0f;
constexpr int MS_PER_FRAME = (int)((1.0f / FPS_LIMIT) * 1000.0f);

CEQD3DWidget::CEQD3DWidget(QWidget* parent): QWidget(parent) {
	m_deviceCreated = false;
	m_renderActive = false;
	m_started = false;
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

CEQD3DWidget::CEQD3DWidget(QWidget* parent, CEPlayground* playground): CEQD3DWidget(parent) {
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

	connect(&m_qTimer, &QTimer::timeout, this, &CEQD3DWidget::OnFrame);

	return true;
}

void CEQD3DWidget::Run() {
	//TODO: Set Widget to edit PER_FRAME
	m_framework->EditorRun(width(), height());
	m_qTimer.start(MS_PER_FRAME);
	m_qETimer.start();
	m_renderActive = m_started = true;
}

void CEQD3DWidget::Release() {
}

void CEQD3DWidget::Pause() {
}

void CEQD3DWidget::Resume() {
}

float CEQD3DWidget::GetMsPerFrame() {
	return m_msPerFrame;
}

bool CEQD3DWidget::event(QEvent* event) {
	switch (event->type()) {
		// Workaround for https://bugreports.qt.io/browse/QTBUG-42183 to get key strokes.
		// To make sure that we always have focus on the widget when we enter the rect area.
	case QEvent::Enter:
	case QEvent::FocusIn:
	case QEvent::FocusAboutToChange:
		qDebug("Enter Direct3D Area");
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
	case QEvent::Leave:
		qDebug("Leave Direct3D Area");
		break;
	case QEvent::KeyPress: {
		auto keyEvent = (QKeyEvent*)event;
		auto nativeKeyEvent = keyEvent->nativeScanCode();
		auto keyCode = keyEvent->key();
		auto virtualKeyCode = keyEvent->nativeVirtualKey();
		auto keyText = keyEvent->text().toUtf8();
		std::string keyChar(keyText.begin(), keyText.end());
		CE_LOG(
			"Native Key Code: " + std::to_string(nativeKeyEvent) +
			" Key Code: " + std::to_string(keyCode) +
			" Virtual Key Code: " + std::to_string(virtualKeyCode) +
			" Char: " + keyChar
		);

		m_framework->EditorKeyDown((WPARAM)keyCode, keyChar);
		emit SignalKeyPressed(reinterpret_cast<QKeyEvent*>(event));
	}
	break;
	case QEvent::MouseMove: {
		auto mouseEvent = reinterpret_cast<QMouseEvent*>(event);
		CE_LOG(
			"Mouse Button Press: " + std::to_string(mouseEvent->button()) + " Mouse Buttons Press: " + std::to_string(
				mouseEvent->buttons()) + " X: " + std::to_string(mouseEvent->x())
			+ " Y: "+ std::to_string(mouseEvent->y()))
		if (mouseEvent->buttons() & Qt::LeftButton) {
			m_framework->EditorMouseMove((WPARAM)mouseEvent->buttons(), mouseEvent->x(), mouseEvent->y());
		}
		emit SignalMouseMoved(mouseEvent);
	}
	break;
	case QEvent::MouseButtonPress: {
		auto mousePressEvent = reinterpret_cast<QMouseEvent*>(event);
		m_framework->EditorMouseUp((WPARAM)mousePressEvent->buttons(), mousePressEvent->x(), mousePressEvent->y());
		emit SignalMouseClicked(mousePressEvent);
	}
	break;
	case QEvent::MouseButtonRelease: {
		auto mouseReleaseEvent = reinterpret_cast<QMouseEvent*>(event);
		m_framework->EditorMouseUp((WPARAM)mouseReleaseEvent->buttons(), mouseReleaseEvent->x(),
		                           mouseReleaseEvent->y());
		emit SignalMouseReleased(mouseReleaseEvent);
	}
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
		emit SignalDeviceCreated(m_deviceCreated);
	}
	QWidget::showEvent(event);
}

QPaintEngine* CEQD3DWidget::paintEngine() const {
	return Q_NULLPTR;
}

void CEQD3DWidget::paintEvent(QPaintEvent* event) {
}

void CEQD3DWidget::resizeEvent(QResizeEvent* event) {
	if (m_deviceCreated) {
		Resize();
	}

	QWidget::resizeEvent(event);
}

void CEQD3DWidget::wheelEvent(QWheelEvent* event) {
}


void CEQD3DWidget::Update() {
	m_framework->EditorUpdateTimer();
	CalculateFPS();
	m_framework->EditorUpdate();

	emit SignalUpdate();
}

void CEQD3DWidget::Render() {
	m_framework->EditorRender();

	emit SignalRender();
}

void CEQD3DWidget::Resize() {

	m_framework->EditorResize(width(), height());

	emit SignalWidgetResize();
}

void CEQD3DWidget::CalculateFPS(bool displayInConsole) {
	static int m_frameCount = 0;
	static float m_timeElapsed = 0.0f;

	m_frameCount++;

	//Compute averages over one second period.
	if ((((m_qETimer.elapsed() + 500) / 1000) - m_timeElapsed) >= 1.0f) {
		m_fps = (float)m_frameCount;
		m_msPerFrame = 1000.0f / m_fps;

		if (displayInConsole) {
			std::stringstream fpsSS;
			fpsSS << "FPS: " << m_fps;
			CE_LOG(fpsSS.str());

			std::stringstream mspfSS;
			mspfSS << "MSPF: " << m_msPerFrame;
			CE_LOG(mspfSS.str());
		}

		m_frameCount = 0;
		m_timeElapsed += 1.0f;
	}
}

float CEQD3DWidget::GetFPS() {
	return m_fps;
}


LRESULT CEQD3DWidget::WndProc(MSG* pMsg) {
	// Process wheel events using Qt's event-system.
	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_MOUSEHWHEEL) return false;

	return false;
}

void CEQD3DWidget::OnFrame() {
	if (m_renderActive)
		Update();
	Render();
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
