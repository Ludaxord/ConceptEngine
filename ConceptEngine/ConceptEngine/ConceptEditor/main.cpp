#include "ConceptEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
    ConceptEditor w;
    // w.resize(1920, 1080);
    w.show();
    return a.exec();

}
