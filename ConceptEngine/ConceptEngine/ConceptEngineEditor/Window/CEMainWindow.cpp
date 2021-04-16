#include "CEMainWindow.h"

#include <QVBoxLayout>

#include "CEDXWindow.h"


CEMainWindow::CEMainWindow(QPlainTextEdit* logWindow) {
	QWidget* wrapper = new QWidget();
	// QWidget* wrapper = QWidget::createWindowContainer(w);
	auto p = wrapper->palette();
	p.setColor(QPalette::Base, QColor(46, 132, 84));
	wrapper->setPalette(p);

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
