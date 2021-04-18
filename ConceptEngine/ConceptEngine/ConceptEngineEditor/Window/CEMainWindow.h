#pragma once
#include <d3d12.h>

#include <QPlainTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "CEConsoleWidget.h"
#include "QDirect3D12Widget.h"

class CEMainWindow : public QWidget {
Q_OBJECT
public:
	explicit CEMainWindow(QPlainTextEdit* logWindow);

	void connectSlots();

private:
	void closeEvent(QCloseEvent* event) override;

public slots:
	void init(bool success);
	void tick();
	void render(ID3D12GraphicsCommandList* cl);

private:
	QVBoxLayout* m_centerLayout;
	QDirect3D12Widget* m_scene;
	QTabWidget* m_infoTab;
	ConceptEngine::Editor::Widgets::CEConsoleWidget* m_info;
};
