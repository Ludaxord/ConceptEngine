#pragma once

#include <d3d12.h>
#include <QtWidgets/QMainWindow>
#include <QCheckBox>
#include "ui_ConceptEngineEditor.h"
#include "Widgets/CEQD3DWidget.h"
#include "Widgets/QDirect3D12Widget.h"
#include "Widgets/CEConsoleWidget.h"

using namespace ConceptEngine::Editor::Widgets;

class ConceptEngineEditor : public QMainWindow {
Q_OBJECT

public:
	ConceptEngineEditor(QWidget* parent = Q_NULLPTR);
	~ConceptEngineEditor();

	void adjustWindowSize();
	void addToolbarWidgets();
	void connectSlots();

	void prepareUi();

	void PrepareDebugConsole();

private:
	void closeEvent(QCloseEvent* event) override;

public slots:
	void init(bool success);
	void tick();
	void render(ID3D12GraphicsCommandList* cl);

private:
	QDirect3D12Widget* m_pScene;
	CEConsoleWidget* m_console = nullptr;

	QSize m_WindowSize;
	QCheckBox* m_pCbxDoFrames;
	QMenuBar* m_menuBar;
	QToolBar* m_mainToolBar;
};
