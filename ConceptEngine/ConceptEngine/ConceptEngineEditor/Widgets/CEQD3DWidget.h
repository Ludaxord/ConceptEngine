#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <stdexcept>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

#include <wrl/client.h>

namespace ConceptEngine::Editor::Widgets {

	class CEQD3DWidget : public QWidget {
	Q_OBJECT

	public:
		CEQD3DWidget(QWidget* parent);
		~CEQD3DWidget();

	private:
		HWND m_hWnd;
	};
}
