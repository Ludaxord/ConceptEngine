#pragma once
#include <d3d12.h>

#include <QPlainTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "CEConsoleWidget.h"
#include "QDirect3D12Widget.h"
#include "../Threads/ThreadLogStream.h"

class CEMainWindow : public QWidget {
Q_OBJECT
public:
	explicit CEMainWindow(QPlainTextEdit* logWindow);

	void connectSlots();
	ConceptEngine::Editor::Widgets::CEConsoleWidget* GetConsole() const;

	static void QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg);

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

	// MessageHandler for display and ThreadLogStream for redirecting cout
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	MessageHandler* msgHandler = Q_NULLPTR;
	ThreadLogStream* m_qd;
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
};
