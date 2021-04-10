#pragma once

#include <d3d12.h>
#include <QtWidgets/QMainWindow>
#include "ui_ConceptEngineEditor.h"

class ConceptEngineEditor : public QMainWindow {
Q_OBJECT

public:
	ConceptEngineEditor(QWidget* parent = Q_NULLPTR);
	~ConceptEngineEditor();

	void Resize();
	void AddToolBarWidgets();
	void ConnectSlots();

protected:

public slots:
	void Create();
	void Update();
	void Render(ID3D12GraphicsCommandList* commandList);

private:
	void closeEvent(QCloseEvent* event) override;

	Ui::ConceptEngineEditorClass ui;

	
};
