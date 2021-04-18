#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "CEQD3DWidget.h"


#include <QtWidgets/QMainWindow>
#include <QDirect3D12Widget.h>


using namespace ConceptEngine::Editor::Widgets;

CEQD3DWidget::CEQD3DWidget(QWidget* parent): QWidget(parent) {
	m_hWnd = reinterpret_cast<HWND>(winId());
}

CEQD3DWidget::~CEQD3DWidget() {
}
