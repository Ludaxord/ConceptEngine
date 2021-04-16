#include "CEMainWindow.h"

#include <QVBoxLayout>

#include "CEDXWindow.h"


CEMainWindow::CEMainWindow(CEDXWindow* w, QPlainTextEdit* logWindow) {
	QWidget* wrapper = QWidget::createWindowContainer(w);

	m_info = new QPlainTextEdit;
	m_info->setReadOnly(true);

	QVBoxLayout* layout = new QVBoxLayout;
	m_infoTab = new QTabWidget(this);
	m_infoTab->addTab(m_info, tr("Machine Info"));
	m_infoTab->addTab(logWindow, tr("Output Log"));
	layout->addWidget(wrapper, 5);
	layout->addWidget(m_infoTab, 2);
	setLayout(layout);
}
