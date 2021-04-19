#pragma once
#include <d3d12.h>

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

	void SingleThreadTest();
	void MultiThreadTest();

private:
	QVBoxLayout* m_centerLayout;
	QDirect3D12Widget* m_scene;
	QTabWidget* m_infoTab;
	ConceptEngine::Editor::Widgets::CEConsoleWidget* m_info;

	// MessageHandler for display and ThreadLogStream for redirecting cout
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	MessageHandler* msgHandler = Q_NULLPTR;
	ThreadLogStream* m_qd;
	QPushButton* m_topButtonMultiThread;
	QPushButton* m_topButtonSingleThread;
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
};
