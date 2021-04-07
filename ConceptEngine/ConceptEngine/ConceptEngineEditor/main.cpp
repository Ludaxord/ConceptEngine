#include "ConceptEngineEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConceptEngineEditor w;
    w.show();
    return a.exec();
}
