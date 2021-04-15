#pragma once

#include <stdexcept>

#include <QWidget>
#include <QTimer>
#include <QPlainTextEdit>

namespace ConceptEngine::Editor::Widgets {
	class CEConsoleWidget : public QPlainTextEdit {
		Q_OBJECT

	signals:
		void getData(const QByteArray& data);
	public:
		explicit CEConsoleWidget(QWidget* parent = nullptr);

		void PutData(const QByteArray& data);
		void SetLocalEchoEnabled(bool echo);

		void DisplayBuildInfo();
	protected:
		void keyPressEvent(QKeyEvent* e) override;
		void mousePressEvent(QMouseEvent* e) override;
		void mouseDoubleClickEvent(QMouseEvent* e) override;
		void contextMenuEvent(QContextMenuEvent* e) override;

	private:
		bool m_localEchoEnabled = false;
	};
}