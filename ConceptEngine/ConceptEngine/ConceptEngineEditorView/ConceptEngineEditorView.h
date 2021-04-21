#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConceptEngineEditorView.h"

class ConceptEngineEditorView : public QMainWindow
{
    Q_OBJECT

public:
    ConceptEngineEditorView(QWidget *parent = Q_NULLPTR);

private:
    Ui::ConceptEngineEditorViewClass ui;
};
