#include "ConceptEngineEditor.h"

#include <QStyle>
#include <QDebug>
#include <QTime>
#include <QScreen>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QLayout>

using namespace ConceptEngine::Editor::Widgets;

ConceptEngineEditor::~ConceptEngineEditor() = default;

void ConceptEngineEditor::adjustWindowSize() {
	resize(
		m_WindowSize.width(),
		m_WindowSize.height()
	);
	setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			size(),
			qApp->screens().first()->availableGeometry()
		)
	);
}

void ConceptEngineEditor::addToolbarWidgets() {
	// Add CheckBox to tool-bar to stop/continue frames execution.
	m_pCbxDoFrames->setText("Do Frames");
	m_pCbxDoFrames->setChecked(true);
	connect(m_pCbxDoFrames,
	        &QCheckBox::stateChanged,
	        [&] {
		        if (m_pCbxDoFrames->isChecked())
			        m_pScene->continueFrames();
		        else
			        m_pScene->pauseFrames();
	        }
	);
	m_mainToolBar->addWidget(m_pCbxDoFrames);
}

void ConceptEngineEditor::connectSlots() {
	connect(m_pScene, &QDirect3D12Widget::deviceInitialized, this, &ConceptEngineEditor::init);
	connect(m_pScene, &QDirect3D12Widget::ticked, this, &ConceptEngineEditor::tick);
	connect(m_pScene, &QDirect3D12Widget::rendered, this, &ConceptEngineEditor::render);

	// NOTE: Additionally, you can listen to some basic IO events.
	// connect(m_pScene, &QDirect3D12Widget::keyPressed, this, &MainWindow::onKeyPressed);
	// connect(m_pScene, &QDirect3D12Widget::mouseMoved, this, &MainWindow::onMouseMoved);
	// connect(m_pScene, &QDirect3D12Widget::mouseClicked, this, &MainWindow::onMouseClicked);
	// connect(m_pScene, &QDirect3D12Widget::mouseReleased, this,
	// &MainWindow::onMouseReleased);
}

void ConceptEngineEditor::prepareUi() {
	m_WindowSize = QSize(1920, 1080);
	m_pCbxDoFrames = new QCheckBox(this);

	this->resize(1920, 1080);

	m_pScene = new QDirect3D12Widget(this);
	m_pScene->setObjectName(QString::fromUtf8("mainWidget"));
	this->setCentralWidget(m_pScene);

	auto menuBarSize = QRect(0, 0, 1920, 30);
	m_menuBar = new QMenuBar(this);
	m_menuBar->setObjectName(QString::fromUtf8("menuBar"));
	m_menuBar->setGeometry(menuBarSize);
	this->setMenuBar(m_menuBar);

	m_mainToolBar = new QToolBar(this);
	m_mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
	this->addToolBar(Qt::TopToolBarArea, m_mainToolBar);

	this->setWindowTitle(QCoreApplication::translate("ConceptEngineEditorClass", "ConceptEngine Editor", nullptr));

	QMetaObject::connectSlotsByName(this);
}

void ConceptEngineEditor::PrepareDebugConsole() {
	auto consoleSize = QRect(0, 0, 1920, 300);
	m_console = new CEConsoleWidget();
	m_console->setGeometry(consoleSize);
	m_console->setReadOnly(true);
	m_console->PutData("Welcome to Concept Engine Editor");
	this->setCentralWidget(m_console);
}

void ConceptEngineEditor::closeEvent(QCloseEvent* event) {
	event->ignore();
	m_pScene->release();
	QTime dieTime = QTime::currentTime().addMSecs(500);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	event->accept();
}

void ConceptEngineEditor::init(bool success) {
	if (!success) {
		QMessageBox::critical(this, "ERROR", "Direct3D widget initialization failed.",
		                      QMessageBox::Ok);
		return;
	}

	// TODO: Add here your extra initialization here.
	// ...

	// Start processing frames with a short delay in case things are still initializing/loading
	// in the background.
	QTimer::singleShot(500, this, [&] {
		m_pScene->run();
	});
	disconnect(m_pScene, &QDirect3D12Widget::deviceInitialized, this, &ConceptEngineEditor::init);
}

void ConceptEngineEditor::tick() {

}

void ConceptEngineEditor::render(ID3D12GraphicsCommandList* cl) {

}

ConceptEngineEditor::ConceptEngineEditor(QWidget* parent) : QMainWindow(parent) {
	prepareUi();
	PrepareDebugConsole();

	adjustWindowSize();
	addToolbarWidgets();
	connectSlots();
}
