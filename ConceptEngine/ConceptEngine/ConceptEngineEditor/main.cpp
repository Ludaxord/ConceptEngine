#include <QLibraryInfo>
#include <QMessageBox>
#include <QApplication>

#include <Windows.h>
#include <spdlog/spdlog.h>

#include "Window/CEMainWindow.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon("F:/Projects/ConceptEngine/assets/conceptenginelogo.png"));

	CEMainWindow mainWindow(true);
	mainWindow.resize(1024, 720);
	mainWindow.show();

	//TODO: Move QMessageOutput to ConsoleWidget
	qInstallMessageHandler(CEMainWindow::QMessageOutput);

	std::cout << "Concept Engine Editor Initialized\n" << std::endl;

	return a.exec();
}
