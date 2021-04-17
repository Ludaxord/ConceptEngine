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
	QWidget* wrapper = new QWidget();

	// setStyleSheet("QWidget { border: 2px solid }");
	QPalette p = palette();
	p.setColor(QPalette::Background, QColor(34, 34, 34));
	p.setColor(QPalette::Text, QColor(192, 192, 192));
	setPalette(p);
	setStyleSheet("border: 5px solid  #3E3E3E;");

	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(46, 132, 84));
	wrapper->setAutoFillBackground(true);
	wrapper->setPalette(pal);

	m_info = new ConceptEngine::Editor::Widgets::CEConsoleWidget();
	m_info->setReadOnly(true);

	m_infoTab = new QTabWidget(this);
	m_infoTab->addTab(m_info, tr("Output Log"));
	// m_infoTab->setStyleSheet(
	// 	"QTabBar::tab { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); border: 2px solid #C4C4C3; border-bottom-color: #C2C7CB; border-top-left-radius: 4px; border-top-right-radius: 4px; min-width: 8ex; padding: 2px;}");
	// m_infoTab->setStyleSheet("QTabBar::tab { background-color: #3E3E3E; color: #BCBCBC }");

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
	meshSwitch->setFocusPolicy(Qt::NoFocus);

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
