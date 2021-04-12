#include "ConceptEngineEditor.h"

#include <QStyle>
#include <QDebug>
#include <QTime>
#include <QScreen>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>

ConceptEngineEditor::~ConceptEngineEditor() {
}

void ConceptEngineEditor::Resize() {
	resize(m_windowSize.width(), m_windowSize.height());
	setGeometry(
		QStyle::alignedRect(Qt::LeftToRight,
		                    Qt::AlignCenter,
		                    size(),
		                    qApp->screens().first()->availableGeometry()
		)
	);
}

void ConceptEngineEditor::AddToolBarWidgets() {
	//Add checkbox to toll-bar to stop/continue frames execution
	m_checkBoxDoFrames->setText("Do Frames");
	m_checkBoxDoFrames->setChecked(true);
	connect(m_checkBoxDoFrames, &QCheckBox::stateChanged, [&] {
		if (m_checkBoxDoFrames->isChecked())
			m_widget->ContinueFrames();
		else
			m_widget->PauseFrames();
	});
	// ui->mainToolBar->addWidget(m_checkBoxDoFrames);
}

void ConceptEngineEditor::ConnectSlots() {
	connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::deviceInitialized, this,
	        &ConceptEngineEditor::Create);
	connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::ticked, this, &ConceptEngineEditor::Update);
	connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::rendered, this, &ConceptEngineEditor::Render);

	// NOTE: Additionally, you can listen to some basic IO events.
	/*
		connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::keyPressed, this, &ConceptEngineEditor::OnKeyPressed);
		connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::mouseMoved, this, &ConceptEngineEditor::OnMouseMoved);
		connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::mouseClicked, this, &ConceptEngineEditor::OnMouseClicked);
		connect(m_widget, &ConceptEngine::Editor::Widgets::CEQD3DWidget::mouseReleased, this, &ConceptEngineEditor::OnMouseReleased); 
	 */

}

void ConceptEngineEditor::Create(bool success) {
	if (!success) {
		QMessageBox::critical(this, "ERROR", "Direct3D widget initialization failed.",
		                      QMessageBox::Ok);
		return;
	}

	QTimer::singleShot(500, this, [&] {
		m_widget->Run();
	});
	disconnect(
		m_widget,
		&ConceptEngine::Editor::Widgets::CEQD3DWidget::deviceInitialized,
		this,
		&ConceptEngineEditor::Create
	);
}

void ConceptEngineEditor::Update() {
}

void ConceptEngineEditor::Render(ID3D12GraphicsCommandList* commandList) {
}

void ConceptEngineEditor::closeEvent(QCloseEvent* event) {
	event->ignore();
	if (m_widget != nullptr) {
		m_widget->Release();
		QTime dieTime = QTime::currentTime().addMSecs(500);
		while (QTime::currentTime() < dieTime)
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	}
	event->accept();
}

ConceptEngineEditor::ConceptEngineEditor(QWidget* parent)
	: QMainWindow(parent) {
	ui = new Ui::ConceptEngineEditorClass();
	m_windowSize = QSize(1920, 1080);
	m_checkBoxDoFrames = new QCheckBox(this);

	ui->setupUi(this);
	m_widget = new ConceptEngine::Editor::Widgets::CEQD3DWidget(this);
	m_widget->setObjectName(QString::fromUtf8("centralWidget"));
	this->setCentralWidget(m_widget);

	if (m_widget != nullptr) {
		Resize();
		AddToolBarWidgets();
		ConnectSlots();
	}

}
