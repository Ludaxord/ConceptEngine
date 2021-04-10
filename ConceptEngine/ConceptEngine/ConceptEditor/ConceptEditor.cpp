#include "ConceptEditor.h"

#include "CEDX12Widget.h"

ConceptEditor::ConceptEditor(QWidget* parent) : QMainWindow(parent), ui(new Ui::ConceptEditorClass) {
	ui->setupUi(this);
	
	// std::unique_ptr<CEDX12Widget> widget = std::make_unique<CEDX12Widget>(this);
	// setCentralWidget(widget.get());
	// widget->Create();
}

ConceptEditor::~ConceptEditor() {
	delete ui;
}

QPaintEngine* ConceptEditor::paintEngine() const {
	return nullptr;
}
