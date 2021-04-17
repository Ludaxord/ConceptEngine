#include "CEMainWindow.h"

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QLCDNumber>

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

	m_infoTab = new QTabWidget(this);
	m_infoTab->addTab(m_info, tr("Machine Info"));
	m_infoTab->addTab(logWindow, tr("Output Log"));

	//debug
	auto infoLabel = new QLabel;
	infoLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
	infoLabel->setAlignment(Qt::AlignCenter);
	infoLabel->setText(tr("This example demonstrates instanced drawing\nof a mesh loaded from a file.\n"
		"Uses a Phong material with a single light.\n"
		"Also demonstrates dynamic uniform buffers\nand a bit of threading with QtConcurrent.\n"
		"Uses 4x MSAA when available.\n"
		"Comes with an FPS camera.\n"
		"Hit [Shift+]WASD to walk and strafe.\nPress and move mouse to look around.\n"
		"Click Add New to increase the number of instances."));

	auto meshSwitch = new QCheckBox(tr("&Use Qt logo"));
	meshSwitch->setFocusPolicy(Qt::NoFocus); // do not interfere with vulkanWindow's keyboard input

	auto counterLcd = new QLCDNumber(5);
	counterLcd->setSegmentStyle(QLCDNumber::Filled);
	counterLcd->display(10);


	QGridLayout* layout = new QGridLayout;
	layout->addWidget(meshSwitch, 0, 0);
	layout->addWidget(counterLcd, 3, 0);
	// setLayout(layout);

	// QVBoxLayout* layout = new QVBoxLayout;
	// layout->addWidget(wrapper, 5);
	// layout->addWidget(m_infoTab, 1);
	// setLayout(layout);

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->addWidget(wrapper, 3);
	vlayout->addWidget(m_infoTab, 1);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->addWidget(infoLabel);
	hlayout->addLayout(vlayout);
	hlayout->addLayout(layout);
	setLayout(hlayout);
}
