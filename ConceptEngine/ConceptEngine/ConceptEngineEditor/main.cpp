#include <QLibraryInfo>
#include <QtWidgets/QApplication>
#include <QPlainTextEdit>
#include <QPointer>

#include "Window/CEDXWindow.h"
#include "Window/CEMainWindow.h"
#include "Window/ConceptEngineEditor.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);

	QPlainTextEdit* messageLogWidget = new QPlainTextEdit(QLatin1String(QLibraryInfo::build()) + QLatin1Char('\n'));
	messageLogWidget->setReadOnly(true);
	CEMainWindow mainWindow(messageLogWidget);
	mainWindow.show();

	// ConceptEngineEditor w;
	// w.show();
	
	return a.exec();
}
