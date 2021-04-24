#include "CEMainWindow.h"

#include <iostream>
#include <QCheckBox>
#include <qcoreapplication.h>
#include <qdialog.h>
#include <qeventloop.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QTime>
#include <spdlog/spdlog.h>

#include "CEDXWindow.h"
#include "CEQD3DWidget.h"

#include "../../ConceptEnginePlaygrounds/Playgrounds/DirectX12/Samples/Shadows/CEDX12ShadowsPlayground.h"

using namespace ConceptEngine::Editor::Widgets;

CEMainWindow::CEMainWindow(bool useCEWidget): m_useCEWidget(useCEWidget) {
	m_centerLayout = new QVBoxLayout();

	if (useCEWidget) {
		m_dxScene = new CEQD3DWidget(m_centerLayout->widget(), new ConceptEngine::Playgrounds::DirectX12::CEDX12ShadowsPlayground());
		m_dxScene->setMinimumSize(320, 240);
	}
	else {
		m_scene = new QDirect3D12Widget(m_centerLayout->widget());
		m_scene->setMinimumSize(320, 240);
	}

	SetEditorPalette();
	CreateMainLayout();
	CreateLogThreadStream();
	connectSlots();
}

void CEMainWindow::connectSlots() {
	connect(m_qd, &ThreadLogStream::sendLogString, msgHandler, &MessageHandler::catchMessage);
	connect(m_topButtonSingleThread, SIGNAL(clicked()), this,SLOT(SingleThreadTest()));
	connect(m_topButtonMultiThread, SIGNAL(clicked()), this,SLOT(MultiThreadTest()));

	if (m_useCEWidget) {
		connect(m_dxScene, &CEQD3DWidget::SignalDeviceCreated, this, &CEMainWindow::init);
		connect(m_dxScene, &CEQD3DWidget::SignalUpdate, this, &CEMainWindow::tick);
		connect(m_dxScene, &CEQD3DWidget::SignalRender, this, &CEMainWindow::Render);
	}
	else {
		connect(m_scene, &QDirect3D12Widget::deviceInitialized, this, &CEMainWindow::init);
		connect(m_scene, &QDirect3D12Widget::ticked, this, &CEMainWindow::tick);
		connect(m_scene, &QDirect3D12Widget::rendered, this, &CEMainWindow::render);
	}
}

ConceptEngine::Editor::Widgets::CEConsoleWidget* CEMainWindow::GetConsole() const {
	return m_info;
}

void CEMainWindow::QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg) {
	std::cout << msg.toStdString().c_str() << std::endl;
	// spdlog::info(msg.toStdString().c_str());
}

void CEMainWindow::closeEvent(QCloseEvent* event) {
	event->ignore();
	if (m_useCEWidget) {
		m_dxScene->Release();
	}
	else {
		m_scene->release();
	}
	QTime dieTime = QTime::currentTime().addMSecs(500);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	event->accept();
}

void CEMainWindow::SetEditorPalette() {
	QPalette p = palette();
	p.setColor(QPalette::Background, QColor(34, 34, 34));
	p.setColor(QPalette::Text, QColor(192, 192, 192));
	setPalette(p);

	auto o = objectName();
	auto styleString = "QHBoxLayout { border: 3px solid #3E3E3E; };";
	setStyleSheet(styleString);

	this->setObjectName("ConceptEngineMainWindow");
	style()->unpolish(this);
	style()->polish(this);
}

QMenuBar* CEMainWindow::CreateTopBarMenu() {

	auto toolbar = new QMenuBar();
	struct CEAction {
		QAction* Action;
		bool Separator = false;
	};

	struct CEMenu {
		std::string Name;
		QMenu* Menu;
		std::vector<CEAction*> Actions;
	};

	std::vector<CEMenu*> menus = {
		new CEMenu{
			"&File", new QMenu(tr("&File")),
			std::vector<CEAction*>{
				new CEAction{new QAction(tr("&New Level..."))},
				new CEAction{new QAction(tr("&Open Level..."))},
				new CEAction{new QAction(tr("&Save"))},
				new CEAction{new QAction(tr("&New Project...")), true},
				new CEAction{new QAction(tr("&Open Project..."))},
				new CEAction{new QAction(tr("&New Component...")), true},
				new CEAction{new QAction(tr("&Exit")), true},
			}
		},
		new CEMenu{
			"&Edit",
			new QMenu(tr("&Edit")),
			std::vector<CEAction*>{
				new CEAction{new QAction(tr("&Undo"))},
				new CEAction{new QAction(tr("&Redo"))},
				new CEAction{new QAction(tr("&Edit Preferences"))},
				new CEAction{new QAction(tr("&Project Settings"))},
			}
		},
		new CEMenu{
			"&Window",
			new QMenu(tr("&Window")),
			std::vector<CEAction*>{
				new CEAction{new QAction(tr("&Enable Fullscreen"))}
			}
		},
		new CEMenu{
			"&Examples",
			new QMenu(tr("&Examples")),
			std::vector<CEAction*>{
				new CEAction{new QAction(tr("&Box Playground"))},
				new CEAction{new QAction(tr("&Basic Shapes Playground"))},
				new CEAction{new QAction(tr("&Basic Waves Playground"))},
				new CEAction{new QAction(tr("&Tessellation Waves Playground"))},
				new CEAction{new QAction(tr("&Blur Waves Playground"))},
				new CEAction{new QAction(tr("&Init Direct3D Playground"))},
				new CEAction{new QAction(tr("&Character Mesh Playground"))},
				new CEAction{new QAction(tr("&Compute Waves Playground"))},
				new CEAction{new QAction(tr("&Cube Map Playground"))},
				new CEAction{new QAction(tr("&Dynamic Cube Playground"))},
				new CEAction{new QAction(tr("&FPP Camera Playground"))},
				new CEAction{new QAction(tr("&Instancing Playground"))},
				new CEAction{new QAction(tr("&Lit Shapes Playground"))},
				new CEAction{new QAction(tr("&Lit Waves Playground"))},
				new CEAction{new QAction(tr("&Normal Map Playground"))},
				new CEAction{new QAction(tr("&Picking Playground"))},
				new CEAction{new QAction(tr("&Shadows Playground"))},
				new CEAction{new QAction(tr("&Sobel Waves Playground"))},
				new CEAction{new QAction(tr("&SSAO Playground"))},
				new CEAction{new QAction(tr("&Stencil Shapes Playground"))},
				new CEAction{new QAction(tr("&Tessellation Waves Playground"))},
				new CEAction{new QAction(tr("&Texture Box Playground"))},
				new CEAction{new QAction(tr("&Tex Waves Playground"))},
				new CEAction{new QAction(tr("&Trees Waves Playground"))}
			}
		},
		new CEMenu{
			"&Help",
			new QMenu(tr("&Help")),
			std::vector<CEAction*>{}
		}
	};

	for (auto menuElement : menus) {
		auto menu = menuElement->Menu;
		auto actions = menuElement->Actions;
		for (auto action : actions) {
			menu->addAction(action->Action);
			if (action->Separator) {
				menu->insertSeparator(action->Action);
			}
		}
		toolbar->addMenu(menu);
	}

	return toolbar;
}

QHBoxLayout* CEMainWindow::CreateActionMenu() {

	auto topLayout = new QHBoxLayout();
	std::string names[] = {
		"Save", "Compile", "Build", "Launch", "Settings"
	};


	for (std::string name : names) {
		auto topButton = new QPushButton(tr(name.c_str()));
		topLayout->addWidget(topButton);
	}

	m_topButtonSingleThread = new QPushButton(tr("Debug Output SingleThread"));
	topLayout->addWidget(m_topButtonSingleThread);

	m_topButtonMultiThread = new QPushButton(tr("Debug Output MultiThread"));
	topLayout->addWidget(m_topButtonMultiThread);

	return topLayout;
}

QGridLayout* CEMainWindow::CreateRightSideMenu() {
	auto meshSwitch = new QCheckBox(tr("&Use Qt logo"));
	meshSwitch->setFocusPolicy(Qt::NoFocus);

	auto counterLcd = new QLCDNumber(5);
	counterLcd->setSegmentStyle(QLCDNumber::Filled);
	counterLcd->display(9999);


	auto* rightLayout = new QGridLayout;
	rightLayout->addWidget(counterLcd, 0, 0);
	rightLayout->addWidget(meshSwitch, 3, 0);
	return rightLayout;
}

QGridLayout* CEMainWindow::CreateLeftSideMenu() {
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
	auto leftLayout = new QGridLayout;
	leftLayout->addWidget(infoLabel, 0, 0);

	return leftLayout;
}

void CEMainWindow::CreateCenterMenu() {

	m_info = new ConceptEngine::Editor::Widgets::CEConsoleWidget();
	m_info->setReadOnly(true);

	m_infoTab = new QTabWidget(this);
	m_infoTab->addTab(m_info, tr("Output Log"));

	m_centerLayout->addLayout(CreateActionMenu(), 0);
	if (m_useCEWidget) {
		m_centerLayout->addWidget(m_dxScene, 3);
	}
	else {
		m_centerLayout->addWidget(m_scene, 3);
	}
	m_centerLayout->addWidget(m_infoTab, 1);
}

QHBoxLayout* CEMainWindow::CreateMainViewLayout() {
	CreateCenterMenu();
	auto mainViewLayout = new QHBoxLayout();
	mainViewLayout->addLayout(CreateLeftSideMenu());
	mainViewLayout->addLayout(m_centerLayout);
	mainViewLayout->addLayout(CreateRightSideMenu());
	return mainViewLayout;
}

void CEMainWindow::CreateLogThreadStream() {
	m_qd = new ThreadLogStream(std::cout);
	this->msgHandler = new MessageHandler(m_info, this);
}

void CEMainWindow::CreateMainLayout() {
	auto mainLayout = new QGridLayout();
	mainLayout->setMenuBar(CreateTopBarMenu());
	mainLayout->addLayout(CreateMainViewLayout(), 0, 0);
	setLayout(mainLayout);
}

void CEMainWindow::init(bool success) {
	if (!success) {
		QMessageBox::critical(m_centerLayout->widget(), "ERROR", "Direct3D widget initialization failed.",
		                      QMessageBox::Ok);
		return;
	}

	QTimer::singleShot(500, m_centerLayout->widget(), [&] {
		if (m_useCEWidget) {
			m_dxScene->Run();
		}
		else {
			m_scene->run();
		}
	});
	if (m_useCEWidget) {
		disconnect(m_dxScene, &CEQD3DWidget::SignalDeviceCreated, this, &CEMainWindow::init);
	}
	else {
		disconnect(m_scene, &QDirect3D12Widget::deviceInitialized, this, &CEMainWindow::init);
	}
}

void CEMainWindow::tick() {

}

void CEMainWindow::render(ID3D12GraphicsCommandList* cl) {
}

void CEMainWindow::Render() {
}

void CEMainWindow::SingleThreadTest() {
	std::cout << "Hello World1" << std::endl;
	qDebug("Hello World1q");
}

void CEMainWindow::MultiThreadTest() {
	QThread* workerThread = new QThread;
	ThreadWorker* worker = new ThreadWorker();
	worker->moveToThread(workerThread);
	connect(workerThread, SIGNAL(started()), worker, SLOT(doWork()));
	connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()));

	connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	workerThread->start();
}
