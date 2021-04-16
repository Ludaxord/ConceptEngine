#include <QLibraryInfo>
#include <QtWidgets/QApplication>
#include <QPlainTextEdit>
#include <QPointer>

#include "Window/CEDXWindow.h"
#include "Window/CEMainWindow.h"
#include "Window/ConceptEngineEditor.h"

static QPointer<QPlainTextEdit> messageLogWidget;

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);

	/*
	ConceptEngineEditor w;
	w.show();
	 */

	//debug
	messageLogWidget = new QPlainTextEdit(QLatin1String(QLibraryInfo::build()) + QLatin1Char('\n'));
	messageLogWidget->setReadOnly(true);
	CEDXWindow* dxWindow = new CEDXWindow;
	CEMainWindow mainWindow(dxWindow, messageLogWidget);

	mainWindow.resize(1024, 768);
	mainWindow.show();

	return a.exec();
}
