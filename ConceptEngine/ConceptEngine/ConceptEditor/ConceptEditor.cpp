#include "ConceptEditor.h"

ConceptEditor::ConceptEditor(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
}

QPaintEngine* ConceptEditor::paintEngine() const {
	return nullptr;
}
