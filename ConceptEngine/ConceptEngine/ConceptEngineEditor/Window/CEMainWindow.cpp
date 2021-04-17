#include "CEMainWindow.h"

#include <QCheckBox>
#include <qdialog.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QMenuBar>
#include <QPushButton>

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


	auto* rightLayout = new QGridLayout;
	rightLayout->addWidget(meshSwitch, 0, 0);
	rightLayout->addWidget(counterLcd, 3, 0);

	auto leftLayout = new QGridLayout;
	leftLayout->addWidget(infoLabel, 0, 0);


	auto topLayout = new QHBoxLayout();
	for (int i = 0; i < 5; i++) {
		auto counter = new QPushButton(tr("Button"));
		topLayout->addWidget(counter);
	}

	auto centerLayout = new QVBoxLayout();
	centerLayout->addLayout(topLayout, 0);
	centerLayout->addWidget(wrapper, 3);
	centerLayout->addWidget(m_infoTab, 1);

	auto toolbar = new QMenuBar();
	QMenu* menus[] = {
		new QMenu(tr("&File")),
		new QMenu(tr("&Edit")),
		new QMenu(tr("&Window")),
		new QMenu(tr("&Help"))
	};
	for (auto menu : menus) {
		toolbar->addMenu(menu);
	}

	auto mainViewLayout = new QHBoxLayout();
	mainViewLayout->addLayout(leftLayout);
	mainViewLayout->addLayout(centerLayout);
	mainViewLayout->addLayout(rightLayout);

	auto mainLayout = new QGridLayout();
	mainLayout->setMenuBar(toolbar);
	mainLayout->addLayout(mainViewLayout, 0, 0);
	setLayout(mainLayout);
}
