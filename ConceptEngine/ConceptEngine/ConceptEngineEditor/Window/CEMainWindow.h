#pragma once
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QWidget>

class CEDXWindow;

class CEMainWindow : public QWidget {
Q_OBJECT
public:
	explicit CEMainWindow(QPlainTextEdit* logWindow);

private:
	CEDXWindow* m_window;
	QTabWidget* m_infoTab;
	QPlainTextEdit* m_info;
};
