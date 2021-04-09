#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConceptEditor.h"
#include <memory>

class ConceptEditor : public QMainWindow
{
    Q_OBJECT

public:
    ConceptEditor(QWidget *parent = Q_NULLPTR);
    ~ConceptEditor();
	
	QPaintEngine* paintEngine() const override;

private:
    Ui::ConceptEditorClass* ui;
};
