#include "ConceptEngineEditorView.h"
#include <QtWidgets/QApplication>
#include <QLibraryInfo>
#include <QMessageBox>

#include <Windows.h>

#include "Window/CEMainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon("F:/Projects/ConceptEngine/assets/conceptenginelogo.png"));


	QPlainTextEdit* messageLogWidget = new QPlainTextEdit(QLatin1String(QLibraryInfo::build()) + QLatin1Char('\n'));
	messageLogWidget->setReadOnly(true);
	CEMainWindow mainWindow(messageLogWidget);
	mainWindow.resize(1024, 720);
	mainWindow.show();

	//TODO: Move QMessageOutput to ConsoleWidget
	qInstallMessageHandler(CEMainWindow::QMessageOutput);

	std::cout << "Concept Engine Editor Initialized\n" << std::endl;

	return a.exec();
}
