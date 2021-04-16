#include "CEMainWindow.h"

#include <QVBoxLayout>

#include "CEDXWindow.h"


CEMainWindow::CEMainWindow(QPlainTextEdit* logWindow) {
	// QWidget* wrapper = QWidget::createWindowContainer(w);
	QWidget* wrapper = new QWidget();
	// wrapper->setMinimumSize(800, 600);
	// wrapper->setGeometry(0, 0, 300, 100);
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(46, 132, 84));
	wrapper->setAutoFillBackground(true);
	wrapper->setPalette(pal);

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
