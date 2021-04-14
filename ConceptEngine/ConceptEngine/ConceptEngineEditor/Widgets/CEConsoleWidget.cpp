#include "CEConsoleWidget.h"
#include <QScrollBar>

using namespace ConceptEngine::Editor::Widgets;

CEConsoleWidget::CEConsoleWidget(QWidget* parent) : QPlainTextEdit(parent) {
	document()->setMaximumBlockCount(100);
	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::gray);
	p.setColor(QPalette::Text, Qt::white);
}

void CEConsoleWidget::PutData(const QByteArray& data) {
	insertPlainText(data);

	QScrollBar* bar = verticalScrollBar();
	bar->setValue(bar->maximum());
}

void CEConsoleWidget::SetLocalEchoEnabled(bool echo) {
	m_localEchoEnabled = echo;
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
