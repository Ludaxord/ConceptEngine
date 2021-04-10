#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConceptEngineEditor.h"

class ConceptEngineEditor : public QMainWindow
{
    Q_OBJECT

public:
    ConceptEngineEditor(QWidget *parent = Q_NULLPTR);

private:
    Ui::ConceptEngineEditorClass ui;
};
