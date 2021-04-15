#include "CEConsoleWidget.h"
#include <QScrollBar>
#include <QLibraryInfo>

using namespace ConceptEngine::Editor::Widgets;

CEConsoleWidget::CEConsoleWidget(QWidget* parent) : QPlainTextEdit(parent) {
	document()->setMaximumBlockCount(100);
	QPalette p = palette();
	p.setColor(QPalette::Base, QColor(34, 34, 34));
	p.setColor(QPalette::Text, QColor(192, 192, 192));
	setPalette(p);

	DisplayBuildInfo();
}

void CEConsoleWidget::PutData(const QByteArray& data) {
	insertPlainText(data);

	QScrollBar* bar = verticalScrollBar();
	bar->setValue(bar->maximum());
}

void CEConsoleWidget::SetLocalEchoEnabled(bool echo) {
	m_localEchoEnabled = echo;
}

void CEConsoleWidget::DisplayBuildInfo() {
	const auto buildInfo = QLibraryInfo::build();
	PutData(buildInfo);
	PutData("\n");
}

void CEConsoleWidget::keyPressEvent(QKeyEvent* e) {
	Q_UNUSED(e);
}

void CEConsoleWidget::mousePressEvent(QMouseEvent* e) {
	Q_UNUSED(e);
}

void CEConsoleWidget::mouseDoubleClickEvent(QMouseEvent* e) {
	Q_UNUSED(e);
}

void CEConsoleWidget::contextMenuEvent(QContextMenuEvent* e) {
	Q_UNUSED(e);
}
