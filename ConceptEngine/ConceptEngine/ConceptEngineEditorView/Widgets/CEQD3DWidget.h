#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <stdexcept>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

#include <wrl/client.h>

namespace ConceptEngineFramework {namespace Graphics {
		class CEPlayground;
	}

	class CEFramework;
}

namespace ConceptEngine::Editor::Widgets {

	class CEQD3DWidget : public QWidget {
	Q_OBJECT

	public:
		CEQD3DWidget(QWidget* parent);
		~CEQD3DWidget();

		void Create();
		void Run();
		void Release();
		void Pause();
		void Resume();
		// Qt Events
	private:
		bool event(QEvent* event) override;
		void showEvent(QShowEvent* event) override;
		QPaintEngine* paintEngine() const override;
		void paintEvent(QPaintEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;

#if QT_VERSION >= 0x050000
		bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#else
    bool winEvent(MSG * message, long * result) override;
#endif
		LRESULT WINAPI WndProc(MSG* pMsg);

		void Update();
		void Render();


	signals:
		void SignalEventHandle();
		void SignalWidgetResize();

		void SignalUpdate();
		void SignalRender();

		void SignalKeyPressed(QKeyEvent*);
		void SignalMouseMoved(QMouseEvent*);
		void SignalMouseClicked(QMouseEvent*);
		void SignalMouseReleased(QMouseEvent*);

	private:
		HWND m_hWnd;
		std::shared_ptr<ConceptEngineFramework::CEFramework> m_framework;
		ConceptEngineFramework::Graphics::CEPlayground* m_playground;
	};
}
