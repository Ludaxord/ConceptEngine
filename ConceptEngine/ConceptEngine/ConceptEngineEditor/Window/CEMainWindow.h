#pragma once
#include <d3d12.h>
#include <QLCDNumber>
#include <QMenuBar>

#include <QPlainTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "CEConsoleWidget.h"
#include "QDirect3D12Widget.h"
#include "../Threads/ThreadLogStream.h"
#include <QObject>
#include <QThread>
#include <QPushButton>

#include "CEQD3DWidget.h"

class ThreadWorker : public QObject {
Q_OBJECT
public:
	ThreadWorker() {
	}

private:
signals:
	void finished();

public slots:
	void doWork() {

		std::cout << "Hello World2" << std::endl;
		qDebug("Hello World2q");
		emit finished();
	}
};


class CEMainWindow : public QWidget {
Q_OBJECT
public:
	explicit CEMainWindow(bool useCEWidget = true);

	void connectSlots();
	ConceptEngine::Editor::Widgets::CEConsoleWidget* GetConsole() const;

	static void QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg);

private:
	void closeEvent(QCloseEvent* event) override;

	void SetEditorPalette();
	void SetDirect3DWindow();
	QMenuBar* CreateTopBarMenu();
	QHBoxLayout* CreateActionMenu();
	QGridLayout* CreateRightSideMenu();
	QGridLayout* CreateLeftSideMenu();
	void CreateCenterMenu();
	QHBoxLayout* CreateMainViewLayout();
	void CreateLogThreadStream();
	void CreateMainLayout();

public slots:
	void init(bool success);
	void tick();
	void render(ID3D12GraphicsCommandList* cl);
	void Render();

	void SingleThreadTest();
	void MultiThreadTest();

private:
	QVBoxLayout* m_centerLayout;

	bool m_useCEWidget;
	QDirect3D12Widget* m_scene;
	ConceptEngine::Editor::Widgets::CEQD3DWidget* m_dxScene;

	QTabWidget* m_infoTab;
	ConceptEngine::Editor::Widgets::CEConsoleWidget* m_info;

	QLCDNumber* m_fpsCounter;
	
	// MessageHandler for display and ThreadLogStream for redirecting cout
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	MessageHandler* msgHandler = Q_NULLPTR;
	ThreadLogStream* m_qd;
	QPushButton* m_topButtonMultiThread;
	QPushButton* m_topButtonSingleThread;
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
};
